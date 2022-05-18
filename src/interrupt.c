#include "interrupt.h"
#include "model.h"
#include "utils.h"

uint8_t avr_get_reg(struct avr *avr, uint16_t reg) {
    return avr->reg[reg];
}

int avr_set_reg(struct avr *avr, uint16_t reg, uint8_t val) {
    avr->reg[reg] = val;
    return 0;
}

static int schedule_interrupt(struct avr *avr, uint32_t vec) {
    avr->status = CPU_STATUS_INTERRUPTING;
    avr->progress = avr->model.interrupt_time;
    avr->reg[avr->model.reg_status] &= 0x7f;
    sim_push(avr, (avr->pc >> 1) & 0xff);
    sim_push(avr, (avr->pc >> 9) & 0xff);
    if (avr->model.pcsize == 3) {
        sim_push(avr, (avr->pc >> 17) & 0xff);
    }
    avr->pc = vec;
}

void avr_check_interrupts(struct avr *avr) {
    if (avr->status == CPU_STATUS_NORMAL && (avr->reg[avr->model.reg_status] & 0x80)) {
        // check timers
        uint32_t vec = avr_find_timer_interrupt(avr);
        if (vec != 0xffffffff) {
            schedule_interrupt(avr, vec);
        }
    }
}
