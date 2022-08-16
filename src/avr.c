#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "model.h"
#include "dispatch.h"
#include "interrupt.h"
#include "opt.h"
#include "avr.h"
#include "utils.h"

static void alloc_avr_memory(struct avr *avr) {
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
    if (avr->mem == NULL) {
        return;
    } else {
        // set up each segment
        avr->reg = avr->mem;
        avr->ram = avr->mem + avr->model.ramstart;
        avr->rom = avr->mem + rom_offset;
        avr->eep = avr->mem + eep_offset;
        memset(avr->reg, 0, avr->model.regsize);
        memset(avr->eep, 0xff, avr->model.eepsize);
    }

    avr_init_flash_state(&avr->flash_data, avr->model.flash_pgsize);
    if (avr->flash_data.buffer == NULL) {
        free(avr->mem);
        avr->mem = NULL;
        return;
    }

    avr_init_eeprom_state(&avr->eeprom_data);

    avr->timer_data = malloc(sizeof(avr->timer_data[0])*avr->model.timer_count);
    if (avr->timer_data == NULL) {
        free(avr->mem);
        free(avr->flash_data.buffer);
        avr->mem = NULL;
    } else {
        for (int i = 0; i < avr->model.timer_count; i++) {
            timerstate_init(avr->timer_data+i);
        }
    }
}

struct avr *avr_init(struct avr_model model) {
    check_compatibility();
    struct avr *avr = malloc(sizeof(*avr));
    if (avr) {
        avr->error = 0;
        avr->status = CPU_STATUS_NORMAL;
        avr->progress = 0;
        avr->model = model;
        avr->pc = 0;
        avr->clock = 0;
        avr->insts = 0;
        avr->pending_inst.type = AVR_PENDING_NONE;

        alloc_avr_memory(avr);

        if (avr->mem == NULL) {
            free(avr);
            avr = NULL;
        } else {
            uint16_t sp = model.ramstart+model.ramsize - 1;
            avr->reg[model.reg_stack+1] = sp >> 8;
            avr->reg[model.reg_stack] = sp & 0xff;
        }
    }
    return avr;
}

void avr_free(struct avr *avr) {
    if (avr) {
        avr_free_flash_state(&avr->flash_data);
        free(avr->mem);
        free(avr->timer_data);
        free(avr);
    }
}

static inline void avr_update(struct avr *avr) {
    avr->clock++;

    avr_update_timers(avr);
    avr_update_eeprom(avr);
    avr_update_flash(avr);

    avr_check_interrupts(avr);
}

static inline void avr_exec(struct avr *avr) {
    uint8_t inst_l = avr->rom[avr->pc],
            inst_h = avr->rom[avr->pc+1];

    avr->insts++;
    LOG("%x:\t", avr->pc);
    avr_dispatch(avr, inst_l, inst_h);
}

static inline void avr_resolve_pending(struct avr *avr) {
    if (avr->pending_inst.type == AVR_PENDING_COPY) {
        if (avr->pending_inst.dst < avr->model.regsize && avr->pending_inst.src < avr->model.regsize) {
            avr_set_reg(avr, avr->pending_inst.dst, avr_get_reg(avr, avr->pending_inst.src));
        } else if (avr->pending_inst.dst < avr->model.regsize) {
            avr_set_reg(avr, avr->pending_inst.dst, avr->mem[avr->pending_inst.src]);
        } else if (avr->pending_inst.src < avr->model.regsize) {
            avr->mem[avr->pending_inst.dst] = avr_get_reg(avr, avr->pending_inst.src);
        } else {
            // not possible within the AVR instruction set but ok
            LOG("*** unexpected memory to memory copy ***\n");
            avr->mem[avr->pending_inst.dst] = avr->mem[avr->pending_inst.src];
        }
        avr->pending_inst.type = AVR_PENDING_NONE;
    }
}

void avr_step(struct avr *avr) {
    switch (avr->status) {
        case CPU_STATUS_NORMAL:
            avr_exec(avr);
            break;

        case CPU_STATUS_CRASHED:
            return;

        case CPU_STATUS_COMPLETING:
            LOG("*** continuing last instruction (%d) ***\n", avr->progress);
            avr->progress--;
            if (avr->progress <= 0) {
                avr_resolve_pending(avr);
                avr->status = CPU_STATUS_NORMAL;
            }
            break;

        case CPU_STATUS_INTERRUPTING:
            LOG("*** responding to interrupt (%d) ***\n", avr->progress);
            avr->progress--;
            if (avr->progress <= 0) {
                avr->status = CPU_STATUS_NORMAL;
            }
            break;

        case CPU_STATUS_IDLE:
            LOG("*** sleeping ***\n");
            break;
    }

    avr_update(avr);
}


uint8_t avr_io_read(struct avr *avr, uint16_t reg) {
    return avr_get_reg(avr, reg);
}

void avr_io_write(struct avr *avr, uint16_t reg, uint8_t val) {
    avr_set_reg(avr, reg, val);
}