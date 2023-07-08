#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "model.h"
#include "dispatch.h"
#include "decode.h"
#include "interrupt.h"
#include "opt.h"
#include "avr.h"
#include "utils.h"
#include "trace.h"

static int alloc_avr_memory(struct avr *avr) {
    // In order to simplify and speed up data accesses, all data segments (register
    // file, SRAM, EEPROM, program memory) are actually just pointers into a
    // single contiguous block of memory, structured to match the address space.
    //
    // However, the program memory and EEPROM often aren't mapped into the data
    // address space so we account for them separately if necessary.
    uint32_t rom_offset = avr->model.romstart,
             eep_offset = avr->model.eepstart,
             unmapped = 0;

    if (avr->model.romstart == 0) {
        rom_offset = avr->model.memend + unmapped;
        unmapped += avr->model.romsize;
    }

    if (avr->model.eepstart == 0) {
        eep_offset = avr->model.memend + unmapped;
        unmapped += avr->model.eepsize;
    }

    avr->mem = malloc(avr->model.memend + unmapped);
    if (avr->mem == NULL) goto fail;

    // set up each segment
    avr->reg = avr->mem;
    avr->ram = avr->mem + avr->model.ramstart;
    avr->rom = avr->mem + rom_offset;
    avr->eep = avr->mem + eep_offset;

    // allocate flash buffer
    if (avr_flash_allocate_internal(avr) == NULL) goto fail;

    // allocate pin connections memory
    avr->pin_data = malloc(sizeof(avr->pin_data[0])*avr->model.port_count);
    if (avr->pin_data == NULL) goto fail;

    // allocate and timers
    avr->timer_data = malloc(sizeof(avr->timer_data[0])*avr->model.timer_count);
    if (avr->timer_data == NULL) goto fail;

    // allocate trace data
    avr->trace = avr_trace_new();
    if (avr->trace == NULL) goto fail;

    return 1;

fail:
    avr_free(avr);
    return 0;
}

struct avr *avr_new(struct avr_model model) {
    struct avr *avr = malloc(sizeof(*avr));
    if (avr) {
        avr->error = 0;
        avr->status = MCU_STATUS_NORMAL;
        avr->progress = 0;
        avr->model = model;
        avr->pc = 0;
        avr->clock = 0;
        avr->insts = 0;
        avr->incomplete_inst.type = AVR_INCOMPLETE_NONE;

        if (alloc_avr_memory(avr) == 0) {
            return NULL; // alloc_avr_memory frees avr as well
        }

        avr_reset(avr);
        avr_io_init(avr);
    }
    return avr;
}

void avr_free(struct avr *avr) {
    if (avr) {
        avr_flash_free_internal(avr);
        avr_trace_free(avr->trace);
        free(avr->mem);
        free(avr->pin_data);
        free(avr->timer_data);
        free(avr);
    }
}

void avr_reset(struct avr *avr) {
    avr->error = 0;
    avr->status = MCU_STATUS_NORMAL;
    avr->progress = 0;
    avr->pc = 0;
    avr->clock = 0;
    avr->insts = 0;
    avr->incomplete_inst.type = AVR_INCOMPLETE_NONE;

    memset(avr->reg, 0x00, avr->model.regsize);
    memset(avr->ram, 0x00, avr->model.ramsize);
    avr_eeprom_reset(avr);
    avr_flash_reset(avr);
    avr_timers_reset(avr);
    avr_trace_reset(avr->trace);
    // pin data is NOT reset, since it is conceptually external to the microcontroller

    uint16_t sp = avr->model.ramstart+avr->model.ramsize - 1;
    avr->reg[avr->model.reg_stack+1] = sp >> 8;
    avr->reg[avr->model.reg_stack] = sp & 0xff;
}

static inline void avr_update(struct avr *avr) {
    avr->clock++;

    avr_update_timers(avr);
    avr_update_eeprom(avr);
    avr_update_flash(avr);

    avr_check_interrupts(avr);
}

static inline void avr_exec(struct avr *avr) {
    uint16_t inst = (avr->rom[avr->pc+1] << 8) | avr->rom[avr->pc];

    LOG("%x:\t", avr->pc);
#ifdef SLIMAVR_DEBUG_HISTORY
    uint16_t inst2 = (avr->rom[avr->pc+3] << 8) | avr->rom[avr->pc+2];
#ifdef SLIMAVR_DEBUG_LOG
    char inst_str[32];
    avr_decode(inst_str, sizeof(inst_str), inst, inst2);
    LOG("%s\n", inst_str);
#endif
    avr_trace_enq(avr->trace, avr->pc, inst, inst2);
#endif
    avr->insts++;
    avr_dispatch(avr, inst);
}

static inline void avr_resolve_incomplete(struct avr *avr) {
    if (avr->incomplete_inst.type == AVR_INCOMPLETE_COPY) {
        if (avr->incomplete_inst.dst < avr->model.regsize && avr->incomplete_inst.src < avr->model.regsize) {
            avr_set_reg(avr, avr->incomplete_inst.dst, avr_get_reg(avr, avr->incomplete_inst.src));
        } else if (avr->incomplete_inst.dst < avr->model.regsize) {
            avr_set_reg(avr, avr->incomplete_inst.dst, avr->mem[avr->incomplete_inst.src]);
        } else if (avr->incomplete_inst.src < avr->model.regsize) {
            avr->mem[avr->incomplete_inst.dst] = avr_get_reg(avr, avr->incomplete_inst.src);
        } else {
            // not possible within the AVR instruction set but ok
            LOG("*** unexpected memory to memory copy ***\n");
            avr->mem[avr->incomplete_inst.dst] = avr->mem[avr->incomplete_inst.src];
        }
        avr->incomplete_inst.type = AVR_INCOMPLETE_NONE;
    }
}

void avr_step(struct avr *avr) {
    switch (avr->status) {
        case MCU_STATUS_NORMAL:
            avr_exec(avr);
            break;

        case MCU_STATUS_COMPLETING:
            LOG("*** continuing last instruction (%d) ***\n", avr->progress);
            avr->progress--;
            if (avr->progress <= 0) {
                avr_resolve_incomplete(avr);
                avr->status = MCU_STATUS_NORMAL;
            }
            break;

        case MCU_STATUS_INTERRUPTING:
            LOG("*** responding to interrupt (%d) ***\n", avr->progress);
            avr->progress--;
            if (avr->progress <= 0) {
                avr->status = MCU_STATUS_NORMAL;
            }
            break;

        case MCU_STATUS_IDLE:
            LOG("*** sleeping ***\n");
            break;

        case MCU_STATUS_CRASHED:
            return;
    }

    avr_update(avr);
}
