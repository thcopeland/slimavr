#include "interrupt.h"
#include "model.h"
#include "utils.h"

static void schedule_interrupt(struct avr *avr, uint32_t vec) {
    if (avr->status == CPU_STATUS_IDLE) {
        avr->progress = avr->model.interrupt_time*2;
    } else {
        avr->progress = avr->model.interrupt_time;
    }
    avr->status = CPU_STATUS_INTERRUPTING;
    avr->reg[avr->model.reg_status] &= 0x7f;
    sim_push(avr, (avr->pc >> 1) & 0xff);
    sim_push(avr, (avr->pc >> 9) & 0xff);
    if (avr->model.pcsize == 3) {
        sim_push(avr, (avr->pc >> 17) & 0xff);
    }
    avr->pc = vec;
}

void avr_check_interrupts(struct avr *avr) {
    if ((avr->status == CPU_STATUS_NORMAL || avr->status == CPU_STATUS_IDLE) &&
        (avr->reg[avr->model.reg_status] & 0x80)) {
        // check timers
        uint32_t vec = avr_find_timer_interrupt(avr);
        if (vec != 0xffffffff) {
            schedule_interrupt(avr, vec);
        } else if ((avr->spm_status & 1) && (avr->reg[avr->model.reg_spmcsr] & 0x80)) {
            avr->spm_status &= ~1;
            schedule_interrupt(avr, avr->model.vec_spmrdy);
        }
    }
}
