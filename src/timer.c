#include "timer.h"
#include "avr.h"

enum mode {
    RESERVED,               // do nothing
    NORMAL,                 // count from 0 to MAX
    CLEAR_ON_COMPARE_ICR,   // count to ICNn
    CLEAR_ON_COMPARE_OCRA,  // count to OCRnA

    // count from 0 to some value, setting/clearing/toggling the OCnx pin when
    // a compare match occurs, then clearing/setting at TOP or BOTTOM
    FAST_PWM_8BIT,          // reset at 0xff
    FAST_PWM_9BIT,          // reset at 0x1ff
    FAST_PWM_10BIT,         // reset at 0x3ff
    FAST_PWM_ICR,           // reset at ICRn
    FAST_PWM_OCRA,          // reset at OCRnA

    // count from 0 to some value, setting/clearing the OCnx pin when a compare
    // match occurs, then count down to 0, clearing/setting the OCnx pin. the OCRnx
    // registers are double buffered and updated at TOP.
    PHASE_PWM_8BIT,         // start counting down at 0xff
    PHASE_PWM_9BIT,         // start counting down at 0x1ff
    PHASE_PWM_10BIT,        // start counting down at 0x3ff
    PHASE_PWM_ICR,          // start counting down at ICRn
    PHASE_PWM_OCRA,         // start counting down at OCRnA

    // identical to phase correct PWM except that OCRnx registers are updated at
    // BOTTOM
    PHASE_FREQ_PWM_ICR,     // start counting down at ICRn
    PHASE_FREQ_PWM_OCRA,    // start counting down at OCRnA
};

static void sync_ocr_regs(struct avr *avr, const struct avr_timer *tmr, uint8_t i) {
    avr->reg[tmr->reg_ocra] = avr->timer_data[i].ocra_buff;
    avr->reg[tmr->reg_ocra+1] = avr->timer_data[i].ocra_buff >> 8;

    // TODO are there ever few than 2 comparators?
    if (tmr->comparators > 1) {
        avr->reg[tmr->reg_ocrb] = avr->timer_data[i].ocrb_buff;
        avr->reg[tmr->reg_ocrb+1] = avr->timer_data[i].ocrb_buff >> 8;

        if (tmr->comparators > 2) {
            avr->reg[tmr->reg_ocrc] = avr->timer_data[i].ocrc_buff;
            avr->reg[tmr->reg_ocrc+1] = avr->timer_data[i].ocrc_buff >> 8;
        }
    }
}

static inline uint16_t get_timer_reg(struct avr *avr, const struct avr_timer *tmr, uint16_t reg) {
    uint16_t val = avr->reg[reg];

    if (tmr->resolution > 8) {
        val |= (uint16_t) avr->reg[reg] >> 8;
    }

    return val;
}


static void timer_tick(struct avr *avr, const struct avr_timer *tmr, uint8_t i, enum mode wgm) {
    uint16_t top, clk = get_timer_reg(avr, tmr, tmr->reg_tcnt);
    uint16_t clock_source = tmr->clock_src[avr->reg[tmr->reg_tccrb] & 0x7];

    if (clock_source == 0 || (clock_source & 0x8000)) {
        // disabled or external clock source
        return;
    } else {
        uint16_t prescale = clock_source & 0x0fff;
        (void) prescale;
        // TODO: prescaled value
    }

    switch (wgm) {
        case RESERVED:
            return;
        case NORMAL:
            sync_ocr_regs(avr, tmr, i);
            top = (1 << tmr->resolution) - 1;
            if (clk == top && avr->reg[tmr->reg_timsk] & tmr->msk_toie) {
                avr->reg[tmr->reg_tifr] |= tmr->msk_tovf;
            }
            if (clk == avr->reg[tmr->reg_ocra] && avr->reg[tmr->reg_timsk] & tmr->msk_ocfiea) {
                avr->reg[tmr->reg_tifr] |= tmr->msk_ocfa;
            }
            if (tmr->comparators > 1 && avr->reg[tmr->reg_timsk] & tmr->msk_ocfieb && clk == avr->reg[tmr->reg_ocrb]) {
                avr->reg[tmr->reg_tifr] |= tmr->msk_ocfb;
            }
            if (tmr->comparators > 2 && avr->reg[tmr->reg_timsk] & tmr->msk_ocfiec && clk == avr->reg[tmr->reg_ocrc]) {
                avr->reg[tmr->reg_tifr] |= tmr->msk_ocfc;
            }
            clk = (clk+1) & top;
            break;
        case CLEAR_ON_COMPARE_ICR:
            sync_ocr_regs(avr, tmr, i);
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case CLEAR_ON_COMPARE_OCRA:
            sync_ocr_regs(avr, tmr, i);
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
        case FAST_PWM_8BIT:
            top = 0xff;
            break;
        case FAST_PWM_9BIT:
            top = 0x1ff;
            break;
        case FAST_PWM_10BIT:
            top = 0x3ff;
            break;
        case FAST_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case FAST_PWM_OCRA:
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
        case PHASE_PWM_8BIT:
            top = 0xff;
            break;
        case PHASE_PWM_9BIT:
            top = 0x1ff;
            break;
        case PHASE_PWM_10BIT:
            top = 0x3ff;
            break;
        case PHASE_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case PHASE_PWM_OCRA:
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
        case PHASE_FREQ_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case PHASE_FREQ_PWM_OCRA:
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
    }

    avr->reg[tmr->reg_tcnt] = clk;

    if (tmr->resolution > 8) {
        avr->reg[tmr->reg_tcnt+1] = clk >> 8;
    }
}

static const enum mode standard_modes[] = {
    NORMAL, PHASE_PWM_8BIT, CLEAR_ON_COMPARE_OCRA, FAST_PWM_8BIT, RESERVED, PHASE_PWM_OCRA, RESERVED, FAST_PWM_OCRA
};

static const enum mode reduced_modes[] = {
    NORMAL, PHASE_PWM_8BIT, PHASE_PWM_9BIT, PHASE_PWM_10BIT, CLEAR_ON_COMPARE_OCRA, FAST_PWM_8BIT, FAST_PWM_9BIT, FAST_PWM_10BIT, PHASE_FREQ_PWM_ICR, PHASE_FREQ_PWM_OCRA, PHASE_PWM_ICR, PHASE_PWM_OCRA, CLEAR_ON_COMPARE_ICR, RESERVED, FAST_PWM_ICR, FAST_PWM_OCRA
};

void avr_update_timers(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        int lookup;
        const struct avr_timer *tmr = avr->model.timers+i;
        // TODO check sleep
        switch (tmr->type) {
            case TIMER_STANDARD:
                lookup = ((avr->reg[tmr->reg_tccrb] & 0x18) >> 1) | (avr->reg[tmr->reg_tccra] & 0x3);
                timer_tick(avr, tmr, i, standard_modes[lookup]);
                break;
            case TIMER_REDUCED:
                lookup = ((avr->reg[tmr->reg_tccrb] & 0x8) >> 1) | (avr->reg[tmr->reg_tccra] & 0x3);
                timer_tick(avr, tmr, i, reduced_modes[lookup]);
                break;
        }
    }
}

uint32_t avr_find_timer_interrupt(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        const struct avr_timer *tmr = avr->model.timers+i;
        // TODO check sleep, any special stuff
        uint8_t tifr = avr->reg[tmr->reg_tifr],
                timsk = avr->reg[tmr->reg_timsk];

        if (tifr == 0) continue;

        if ((tifr & tmr->msk_tovf) && (timsk & tmr->msk_toie)) {
            avr->reg[tmr->reg_tifr] &= ~tmr->msk_tovf;
            return tmr->vec_ovf;
        }

        if ((tifr & tmr->msk_ocfa) && (timsk & tmr->msk_ocfiea)) {
            avr->reg[tmr->reg_tifr] &= ~tmr->msk_ocfa;
            return tmr->vec_compa;
        }

        if (tmr->comparators > 1) {
            if ((tifr & tmr->msk_ocfb) && (timsk & tmr->msk_ocfieb)) {
                avr->reg[tmr->reg_tifr] &= ~tmr->msk_ocfb;
                return tmr->vec_compb;
            }

            if (tmr->comparators > 2) {
                if ((tifr & tmr->msk_ocfc) && (timsk & tmr->msk_ocfiec)) {
                    avr->reg[tmr->reg_tifr] &= ~tmr->msk_ocfc;
                    return tmr->vec_compc;
                }
            }
        }
    }

    return 0xffffffff;
}
