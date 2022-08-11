#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "model.h"
#include "dispatch.h"
#include "interrupt.h"
#include "opt.h"
#include "avr.h"

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
    }

    avr->flash_pgbuff = NULL;
    avr->timer_data = NULL;

    avr->flash_pgbuff = malloc(avr->model.flash_pgsize);
    if (avr->flash_pgbuff == NULL) goto nomem;

    avr->timer_data = malloc(sizeof(avr->timer_data[0])*avr->model.timer_count);
    if (avr->timer_data == NULL) {
        goto nomem;
    } else {
        for (int i = 0; i < avr->model.timer_count; i++) {
            timerstate_init(avr->timer_data+i);
        }
    }

    return;

nomem:
    free(avr->mem);
    free(avr->flash_pgbuff);
    avr->mem = NULL;
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
        free(avr->mem);
        free(avr->flash_pgbuff);
        free(avr->timer_data);
        free(avr);
    }
}

static inline void avr_update(struct avr *avr) {
    avr->clock++;

    avr_update_timers(avr);
    avr_update_eeprom(avr);

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

// map between register type and timer index
#define reg_type_timer(type) (((int)type-5)/2)

// ensure that the mapping remains correct
static_assert (reg_type_timer(REG_TIMER0_HIGH) == 0);
static_assert (reg_type_timer(REG_TIMER0_LOW) == 0);
static_assert (reg_type_timer(REG_TIMER1_HIGH) == 1);
static_assert (reg_type_timer(REG_TIMER1_LOW) == 1);
static_assert (reg_type_timer(REG_TIMER2_HIGH) == 2);
static_assert (reg_type_timer(REG_TIMER2_LOW) == 2);
static_assert (reg_type_timer(REG_TIMER3_HIGH) == 3);
static_assert (reg_type_timer(REG_TIMER3_LOW) == 3);
static_assert (reg_type_timer(REG_TIMER4_HIGH) == 4);
static_assert (reg_type_timer(REG_TIMER4_LOW) == 4);
static_assert (reg_type_timer(REG_TIMER5_HIGH) == 5);
static_assert (reg_type_timer(REG_TIMER5_LOW) == 5);

uint8_t avr_get_reg(struct avr *avr, uint16_t reg) {
    enum avr_register_type type = avr->model.regmap[reg].type;

    switch (type) {
        case REG_RESERVED:
            return 0xff;

        case REG_VALUE:
        case REG_UNSUPPORTED:
        case REG_CLEAR_ON_SET:
        case REG_EEP_CONTROL:
            return avr->reg[reg];

        case REG_TIMER0_LOW:
        case REG_TIMER1_LOW:
        case REG_TIMER2_LOW:
        case REG_TIMER3_LOW:
        case REG_TIMER4_LOW:
        case REG_TIMER5_LOW:
            avr->timer_data[reg_type_timer(type)].tmp = avr->reg[reg+1];
            return avr->reg[reg];

        case REG_TIMER0_HIGH:
        case REG_TIMER1_HIGH:
        case REG_TIMER2_HIGH:
        case REG_TIMER3_HIGH:
        case REG_TIMER4_HIGH:
        case REG_TIMER5_HIGH:
            return avr->timer_data[reg_type_timer(type)].tmp;

        default:
            assert(0); // should be comprehensive
    }
}

void avr_set_reg(struct avr *avr, uint16_t reg, uint8_t val) {
    enum avr_register_type type = avr->model.regmap[reg].type;

    switch (type) {
        case REG_RESERVED:
            break;

        case REG_VALUE:
        case REG_UNSUPPORTED:
            avr->reg[reg] = val;
            break;

        case REG_CLEAR_ON_SET:
            avr->reg[reg] &= ~val;
            break;

        case REG_EEP_CONTROL:
            avr_set_eeprom_reg(avr, reg, val);
            break;

        case REG_TIMER0_LOW:
        case REG_TIMER1_LOW:
        case REG_TIMER2_LOW:
        case REG_TIMER3_LOW:
        case REG_TIMER4_LOW:
        case REG_TIMER5_LOW:
            avr->reg[reg+1] = avr->timer_data[reg_type_timer(type)].tmp;
            avr->reg[reg] = val;
            break;

        case REG_TIMER0_HIGH:
        case REG_TIMER1_HIGH:
        case REG_TIMER2_HIGH:
        case REG_TIMER3_HIGH:
        case REG_TIMER4_HIGH:
        case REG_TIMER5_HIGH:
            avr->timer_data[reg_type_timer(type)].tmp = val;
            break;

        default:
            assert(0); // should be comprehensive
    }
}
