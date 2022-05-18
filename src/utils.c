#include "utils.h"

static inline uint16_t get_sp(struct avr *avr) {
    uint8_t reg = avr->model.reg_stack;

    if (avr->model.ramsize > 256) {
        return ((uint16_t) avr->reg[reg+1] << 8) | (avr->reg[reg]);
    } else {
        return avr->reg[reg];
    }
}

static inline void set_sp(struct avr *avr, uint16_t sp) {
    uint8_t reg = avr->model.reg_stack;
    if (avr->model.ramsize > 256) {
        avr->reg[reg+1] = sp >> 8;
    }
    avr->reg[reg] = sp;
}

void sim_push(struct avr *avr, uint8_t val) {
    uint16_t sp = get_sp(avr);

    if (sp < avr->model.ramstart+avr->model.ramsize) {
        avr->mem[sp] = val;
        set_sp(avr, --sp);
    } else {
        avr->error = CPU_INVALID_RAM_ADDRESS;
        avr->status = CPU_STATUS_CRASHED;
    }
}

uint8_t sim_pop(struct avr *avr) {
    uint16_t sp = get_sp(avr);

    if (sp < avr->model.ramstart+avr->model.ramsize) {
        set_sp(avr, ++sp);
        return avr->mem[sp];
    } else {
        avr->error = CPU_INVALID_RAM_ADDRESS;
        avr->status = CPU_STATUS_CRASHED;
        return 0;
    }
}
