#include "timer.h"
#include "avr.h"

void timerstate_init(struct avr_timerstate *state) {
    state->prescale_clock = 0;
    state->ocra_high = 0;
    state->ocra_low = 0;
    state->ocrb_high = 0;
    state->ocrb_low = 0;
    state->ocrc_high = 0;
    state->ocrc_low = 0;
    state->tmp = 0;
    state->delta = 1;
}

static inline uint16_t get_timer_reg(struct avr *avr, const struct avr_timer *tmr, uint16_t reg) {
    uint16_t val = avr->reg[reg];

    if (tmr->resolution > 8) {
        val |= (uint16_t) avr->reg[reg+1] << 8;
    }

    return val;
}

// must match avr_timer_cs order
static const uint16_t prescale_mask_table[] = {
    0, 0x000, 0x001, 0x003, 0x007, 0x00f, 0x01f, 0x03f, 0x07f, 0xff, 0x1ff, 0x3ff, 0, 0
};

static void timer_tick(struct avr *avr, const struct avr_timer *tmr, struct avr_timerstate *state, enum avr_timer_wgm wgm, enum avr_timer_cs cs, uint8_t comvals) {
    // various behavior variables
    //  top - max value
    //  ovf - overflow interrupt value
    //  sync - when to synchronize double-buffered values
    //  clk - current value of the timer
    //  rev - when to reverse counting direction (either top, 0, or 0xffffffff (no reverse))
    //  com_table - which compare output mode table to use
    //  coma/b/c - compare output modes for each output pin (set to COM_SPECIAL for special cases)
    uint16_t top, ovf, sync, clk = get_timer_reg(avr, tmr, tmr->reg_tcnt);
    uint32_t rev = 0xffffffff;
    const enum avr_timer_com *com_table;
    enum avr_timer_com coma = COM_DISCONNECTED,
                       comb = COM_DISCONNECTED,
                       comc = COM_DISCONNECTED;

    if (cs == CS_DISABLED || cs == CS_FALLING || cs == CS_RISING) {
        return; // disabled or external clock source
    } else {
        uint16_t mask = prescale_mask_table[cs];
        if (((++state->prescale_clock) & mask) != 0) return;
    }

    uint16_t ocra = state->ocra_low | (tmr->resolution > 8 ? state->ocra_high << 8 : 0),
             ocrb = state->ocrb_low | (tmr->resolution > 8 ? state->ocrb_high << 8 : 0),
             ocrc = state->ocrc_low | (tmr->resolution > 8 ? state->ocrc_high << 8 : 0);

    switch (wgm) {
        case WGM_RESERVED:
            return;
        case WGM_NORMAL:
            sync = clk;
            top = (1 << tmr->resolution) - 1;
            ovf = top;
            com_table = tmr->com_non_pwm_table;
            break;
        case WGM_CLEAR_ON_COMPARE_ICR:
            sync = clk;
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            ovf = (1 << tmr->resolution) - 1;
            com_table = tmr->com_non_pwm_table;
            break;
        case WGM_CLEAR_ON_COMPARE_OCRA:
            sync = clk;
            top = ocra;
            ovf = (1 << tmr->resolution) - 1;
            com_table = tmr->com_non_pwm_table;
            break;
        case WGM_FAST_PWM_8BIT:
            top = 0xff;
            sync = 0;
            ovf = top;
            com_table = tmr->com_fast_pwm_table;
            break;
        case WGM_FAST_PWM_9BIT:
            top = 0x1ff;
            sync = 0;
            ovf = top;
            com_table = tmr->com_fast_pwm_table;
            break;
        case WGM_FAST_PWM_10BIT:
            top = 0x3ff;
            sync = 0;
            ovf = top;
            com_table = tmr->com_fast_pwm_table;
            break;
        case WGM_FAST_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            sync = 0;
            ovf = top;
            com_table = tmr->com_fast_pwm_table;
            break;
        case WGM_FAST_PWM_OCRA:
            top = ocra;
            sync = 0;
            ovf = top;
            com_table = tmr->com_fast_pwm_table;
            break;
        case WGM_PHASE_PWM_8BIT:
            top = 0xff;
            sync = top;
            rev = top;
            ovf = 0;
            com_table = tmr->com_phase_pwm_table;
            break;
        case WGM_PHASE_PWM_9BIT:
            top = 0x1ff;
            sync = top;
            rev = top;
            ovf = 0;
            com_table = tmr->com_phase_pwm_table;
            break;
        case WGM_PHASE_PWM_10BIT:
            top = 0x3ff;
            sync = top;
            rev = top;
            ovf = 0;
            com_table = tmr->com_phase_pwm_table;
            break;
        case WGM_PHASE_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            sync = top;
            ovf = 0;
            com_table = tmr->com_phase_pwm_table;
            break;
        case WGM_PHASE_PWM_OCRA:
            top = ocra;
            sync = top;
            rev = top;
            ovf = 0;
            com_table = tmr->com_phase_pwm_table;
            break;
        case WGM_PHASE_FREQ_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            sync = 0;
            rev = top;
            ovf = 0;
            com_table = tmr->com_phase_pwm_table;
            break;
        case WGM_PHASE_FREQ_PWM_OCRA:
            top = ocra;
            sync = 0;
            rev = top;
            ovf = 0;
            com_table = tmr->com_phase_pwm_table;
            break;
    }

    if (sync == clk) {
        uint16_t prev_ocra = ocra;
        ocra = state->ocra_low = avr->reg[tmr->reg_ocra];
        if (tmr->resolution > 8) {
            state->ocra_high = avr->reg[tmr->reg_ocra+1];
            ocra |= state->ocra_high << 8;
        }
        if (prev_ocra == rev) rev = ocra;

        // TODO are there ever few than 2 comparators?
        if (tmr->comparators > 1) {
            ocrb = state->ocrb_low = avr->reg[tmr->reg_ocrb];
            if (tmr->resolution > 8) {
                state->ocrb_high = avr->reg[tmr->reg_ocrb+1];
                ocrb |= state->ocrb_high << 8;
            }

            if (tmr->comparators > 2) {
                ocrc = state->ocrc_low = avr->reg[tmr->reg_ocrc];
                if (tmr->resolution > 8) {
                    state->ocrc_high = avr->reg[tmr->reg_ocrc+1];
                    ocrc |= state->ocrc_high << 8;
                }
            }
        }
    }

    coma = com_table[(comvals>>6)];
    if (tmr->comparators > 1) comb = com_table[(comvals>>4) & 0x03];
    if (tmr->comparators > 2) comc = com_table[(comvals>>2) & 0x03];

    // handle special cases (ocra/b/c == 0 or top)
    switch (wgm) {
        case WGM_FAST_PWM_8BIT:
        case WGM_FAST_PWM_9BIT:
        case WGM_FAST_PWM_10BIT:
        case WGM_FAST_PWM_ICR:
        case WGM_FAST_PWM_OCRA:
            if (coma != COM_DISCONNECTED) {
                if (ocra == 0) {
                    if (clk == 0) avr->reg[tmr->reg_oca] |= tmr->msk_oca;
                    else avr->reg[tmr->reg_oca] &= ~tmr->msk_oca;
                    coma = COM_SPECIAL;
                } else if (ocra == top) {
                    if (coma == COM_INVERTING) avr->reg[tmr->reg_oca] |= tmr->msk_oca;
                    else avr->reg[tmr->reg_oca] &= ~tmr->msk_oca;
                    coma = COM_SPECIAL;
                }
            }

            if (comb != COM_DISCONNECTED) {
                if (ocrb == 0) {
                    if (clk == 0) avr->reg[tmr->reg_ocb] |= tmr->msk_ocb;
                    else avr->reg[tmr->reg_ocb] &= ~tmr->msk_ocb;
                    comb = COM_SPECIAL;
                } else if (ocrb == top) {
                    if (comb == COM_INVERTING) avr->reg[tmr->reg_ocb] |= tmr->msk_ocb;
                    else avr->reg[tmr->reg_ocb] &= ~tmr->msk_ocb;
                    comb = COM_SPECIAL;
                }
            }

            if (comc != COM_DISCONNECTED) {
                if (ocrc == 0) {
                    if (clk == 0) avr->reg[tmr->reg_occ] |= tmr->msk_occ;
                    else avr->reg[tmr->reg_occ] &= ~tmr->msk_occ;
                    comc = COM_SPECIAL;
                } else if (ocrc == top) {
                    if (comc == COM_INVERTING) avr->reg[tmr->reg_occ] |= tmr->msk_occ;
                    else avr->reg[tmr->reg_occ] &= ~tmr->msk_occ;
                    comc = COM_SPECIAL;
                }
            }
            break;
        case WGM_PHASE_PWM_8BIT:
        case WGM_PHASE_PWM_9BIT:
        case WGM_PHASE_PWM_10BIT:
        case WGM_PHASE_PWM_ICR:
        case WGM_PHASE_PWM_OCRA:
        case WGM_PHASE_FREQ_PWM_ICR:
        case WGM_PHASE_FREQ_PWM_OCRA:
            if ((ocra == 0 && (coma == COM_NON_INVERTING || coma == COM_CLEAR_UP_SET_DOWN)) ||
                (ocra == top && (coma == COM_INVERTING || coma == COM_SET_UP_CLEAR_DOWN))) {
                avr->reg[tmr->reg_oca] &= ~tmr->msk_oca;
                coma = COM_SPECIAL;
            } else if ((ocra == 0 && (coma == COM_INVERTING || coma == COM_SET_UP_CLEAR_DOWN)) ||
                (ocra == top && (coma == COM_NON_INVERTING || coma == COM_CLEAR_UP_SET_DOWN))) {
                avr->reg[tmr->reg_oca] |= tmr->msk_oca;
                coma = COM_SPECIAL;
            }

            if ((ocrb == 0 && (comb == COM_NON_INVERTING || comb == COM_CLEAR_UP_SET_DOWN)) ||
                (ocrb == top && (comb == COM_INVERTING || comb == COM_SET_UP_CLEAR_DOWN))) {
                avr->reg[tmr->reg_ocb] &= ~tmr->msk_ocb;
                comb = COM_SPECIAL;
            } else if ((ocrb == 0 && (comb == COM_INVERTING || comb == COM_SET_UP_CLEAR_DOWN)) ||
                (ocrb == top && (comb == COM_NON_INVERTING || comb == COM_CLEAR_UP_SET_DOWN))) {
                avr->reg[tmr->reg_ocb] |= tmr->msk_ocb;
                comb = COM_SPECIAL;
            }

            if ((ocrc == 0 && (comc == COM_NON_INVERTING || comc == COM_CLEAR_UP_SET_DOWN)) ||
                (ocrc == top && (comc == COM_INVERTING || comc == COM_SET_UP_CLEAR_DOWN))) {
                avr->reg[tmr->reg_occ] &= ~tmr->msk_occ;
                comc = COM_SPECIAL;
            } else if ((ocrc == 0 && (comc == COM_INVERTING || comc == COM_SET_UP_CLEAR_DOWN)) ||
                (ocrc == top && (comc == COM_NON_INVERTING || comc == COM_CLEAR_UP_SET_DOWN))) {
                avr->reg[tmr->reg_occ] |= tmr->msk_occ;
                comc = COM_SPECIAL;
            }
            break;
        default:
            break;
    }

    if (clk == 0) {
        if (coma == COM_NON_INVERTING)  avr->reg[tmr->reg_oca] |= tmr->msk_oca;
        else if (coma == COM_INVERTING) avr->reg[tmr->reg_oca] &= ~tmr->msk_oca;
        if (comb == COM_NON_INVERTING)  avr->reg[tmr->reg_ocb] |= tmr->msk_ocb;
        else if (comb == COM_INVERTING) avr->reg[tmr->reg_ocb] &= ~tmr->msk_ocb;
        if (comc == COM_NON_INVERTING)  avr->reg[tmr->reg_occ] |= tmr->msk_occ;
        else if (comc == COM_INVERTING) avr->reg[tmr->reg_occ] &= ~tmr->msk_occ;
    }

    if (clk == ovf) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_tovf;
    }

    if (clk == ocra) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_ocfa;

        if (coma != COM_DISCONNECTED && (clk != top || coma == COM_TOGGLE || coma == COM_CLEAR || coma == COM_SET)) {
            if (coma == COM_TOGGLE) avr->reg[tmr->reg_oca] ^= tmr->msk_oca;
            else if (coma == COM_CLEAR || coma == COM_NON_INVERTING ||
                     (coma == COM_CLEAR_UP_SET_DOWN && state->delta > 0) ||
                     (coma == COM_SET_UP_CLEAR_DOWN && state->delta < 0)) {
                avr->reg[tmr->reg_oca] &= ~tmr->msk_oca;
            } else if (coma == COM_SET || coma == COM_INVERTING ||
                     (coma == COM_CLEAR_UP_SET_DOWN && state->delta < 0) ||
                     (coma == COM_SET_UP_CLEAR_DOWN && state->delta > 0)) {
                avr->reg[tmr->reg_oca] |= tmr->msk_oca;
            }
        }
    }

    if (tmr->comparators > 1 && clk == ocrb) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_ocfb;

        if (comb != COM_DISCONNECTED && (clk != top || comb == COM_TOGGLE || comb == COM_CLEAR || comb == COM_SET)) {
            if (comb == COM_TOGGLE) avr->reg[tmr->reg_ocb] ^= tmr->msk_ocb;
            else if (comb == COM_CLEAR || comb == COM_NON_INVERTING ||
                     (comb == COM_CLEAR_UP_SET_DOWN && state->delta > 0) ||
                     (comb == COM_SET_UP_CLEAR_DOWN && state->delta < 0)) {
                avr->reg[tmr->reg_ocb] &= ~tmr->msk_ocb;
            } else if (comb == COM_SET || comb == COM_INVERTING ||
                     (comb == COM_CLEAR_UP_SET_DOWN && state->delta < 0) ||
                     (comb == COM_SET_UP_CLEAR_DOWN && state->delta > 0)) {
                avr->reg[tmr->reg_ocb] |= tmr->msk_ocb;
            }
        }
    }

    if (tmr->comparators > 2 && clk == ocrc) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_ocfc;

        if (comc != COM_DISCONNECTED && (clk != top || comc == COM_TOGGLE || comc == COM_CLEAR || comc == COM_SET)) {
            if (comc == COM_TOGGLE) avr->reg[tmr->reg_occ] ^= tmr->msk_occ;
            else if (comc == COM_CLEAR || comc == COM_NON_INVERTING ||
                     (comc == COM_CLEAR_UP_SET_DOWN && state->delta > 0) ||
                     (comc == COM_SET_UP_CLEAR_DOWN && state->delta < 0)) {
                avr->reg[tmr->reg_occ] &= ~tmr->msk_occ;
            } else if (comc == COM_SET || comc == COM_INVERTING ||
                     (comc == COM_CLEAR_UP_SET_DOWN && state->delta < 0) ||
                     (comc == COM_SET_UP_CLEAR_DOWN && state->delta > 0)) {
                avr->reg[tmr->reg_occ] |= tmr->msk_occ;
            }
        }
    }

    clk = clk+state->delta;
    if (clk > top) { // catches underflow as well
        clk = 0;
        state->delta = 1;
    }

    if (clk == rev) {
        state->delta = state->delta > 0 ? -1 : 1;
    }

    avr->reg[tmr->reg_tcnt] = clk;
    if (tmr->resolution > 8) {
        avr->reg[tmr->reg_tcnt+1] = clk >> 8;
    }
}

void avr_update_timers(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        const struct avr_timer *tmr = avr->model.timers+i;
        uint8_t wgmidx, csidx, comvals;
        uint8_t tccrb = avr->reg[tmr->reg_tccrb];

        // quick exit (a little hacky but significantly improves performance)
        if (tccrb == 0) continue;

        // TODO check sleep
        switch (tmr->type) {
            case TIMER_STANDARD:
                wgmidx = ((avr->reg[tmr->reg_tccrb] & 0x18) >> 1) | (avr->reg[tmr->reg_tccra] & 0x3);
                csidx = avr->reg[tmr->reg_tccrb] & 0x07;
                break;
            case TIMER_REDUCED:
                wgmidx = ((avr->reg[tmr->reg_tccrb] & 0x8) >> 1) | (avr->reg[tmr->reg_tccra] & 0x3);
                csidx = avr->reg[tmr->reg_tccrb] & 0x07;
                break;
        }

        comvals = avr->reg[tmr->reg_tccra] & 0xfc;
        timer_tick(avr, tmr, avr->timer_data+i, tmr->wgm_table[wgmidx], tmr->clock_src_table[csidx], comvals);
    }
}

uint32_t avr_find_timer_interrupt(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        const struct avr_timer *tmr = avr->model.timers+i;
        // TODO check sleep, any special stuff
        uint8_t tifr = avr->reg[tmr->reg_tifr],
                timsk = avr->reg[tmr->reg_timsk],
                tccrb = avr->reg[tmr->reg_tccrb];

        if (tifr == 0 || tccrb == 0) continue;

        if ((tifr & tmr->msk_tovf) && (timsk & tmr->msk_toie)) {
            avr->reg[tmr->reg_tifr] &= ~tmr->msk_tovf;
            return tmr->vec_ovf;
        }

        if ((tifr & tmr->msk_ocfa) && (timsk & tmr->msk_ociea)) {
            avr->reg[tmr->reg_tifr] &= ~tmr->msk_ocfa;
            return tmr->vec_compa;
        }

        if (tmr->comparators > 1) {
            if ((tifr & tmr->msk_ocfb) && (timsk & tmr->msk_ocieb)) {
                avr->reg[tmr->reg_tifr] &= ~tmr->msk_ocfb;
                return tmr->vec_compb;
            }

            if (tmr->comparators > 2) {
                if ((tifr & tmr->msk_ocfc) && (timsk & tmr->msk_ociec)) {
                    avr->reg[tmr->reg_tifr] &= ~tmr->msk_ocfc;
                    return tmr->vec_compc;
                }
            }
        }
    }

    return 0xffffffff;
}
