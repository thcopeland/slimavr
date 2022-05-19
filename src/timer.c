#include "timer.h"
#include "avr.h"

static void sync_ocr_regs(struct avr *avr, const struct avr_timer *tmr, uint8_t i) {
    avr->reg[tmr->reg_ocra] = avr->timer_data[i].ocral_buff;
    avr->reg[tmr->reg_ocra+1] = avr->timer_data[i].ocrah_buff;

    // TODO are there ever few than 2 comparators?
    if (tmr->comparators > 1) {
        avr->reg[tmr->reg_ocrb] = avr->timer_data[i].ocrbl_buff;
        avr->reg[tmr->reg_ocrb+1] = avr->timer_data[i].ocrbh_buff;

        if (tmr->comparators > 2) {
            avr->reg[tmr->reg_ocrc] = avr->timer_data[i].ocrcl_buff;
            avr->reg[tmr->reg_ocrc+1] = avr->timer_data[i].ocrch_buff;
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

// must match avr_timer_cs order
static const uint16_t prescale_mask_table[] = {
    0, 0x000, 0x001, 0x003, 0x007, 0x00f, 0x01f, 0x03f, 0x07f, 0xff, 0x1ff, 0x3ff, 0, 0
};

static void timer_tick(struct avr *avr, const struct avr_timer *tmr, uint8_t i, enum avr_timer_wgm wgm, enum avr_timer_cs cs) {
    uint16_t top, clk = get_timer_reg(avr, tmr, tmr->reg_tcnt);

    if (cs == CS_DISABLED || cs == CS_FALLING || cs == CS_RISING) {
        return; // disabled or external clock source
    } else {
        uint16_t mask = prescale_mask_table[cs];
        if (((++avr->timer_data[i].prescale_clock) & mask) != 0) return;
    }

    switch (wgm) {
        case WGM_RESERVED:
            return;
        case WGM_NORMAL:
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
        case WGM_CLEAR_ON_COMPARE_ICR:
            sync_ocr_regs(avr, tmr, i);
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case WGM_CLEAR_ON_COMPARE_OCRA:
            sync_ocr_regs(avr, tmr, i);
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
        case WGM_FAST_PWM_8BIT:
            top = 0xff;
            break;
        case WGM_FAST_PWM_9BIT:
            top = 0x1ff;
            break;
        case WGM_FAST_PWM_10BIT:
            top = 0x3ff;
            break;
        case WGM_FAST_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case WGM_FAST_PWM_OCRA:
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
        case WGM_PHASE_PWM_8BIT:
            top = 0xff;
            break;
        case WGM_PHASE_PWM_9BIT:
            top = 0x1ff;
            break;
        case WGM_PHASE_PWM_10BIT:
            top = 0x3ff;
            break;
        case WGM_PHASE_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case WGM_PHASE_PWM_OCRA:
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
        case WGM_PHASE_FREQ_PWM_ICR:
            top = get_timer_reg(avr, tmr, tmr->reg_icr);
            break;
        case WGM_PHASE_FREQ_PWM_OCRA:
            top = get_timer_reg(avr, tmr, tmr->reg_ocra);
            break;
    }

    avr->reg[tmr->reg_tcnt] = clk;

    if (tmr->resolution > 8) {
        avr->reg[tmr->reg_tcnt+1] = clk >> 8;
    }
}

void avr_update_timers(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        const struct avr_timer *tmr = avr->model.timers+i;
        uint8_t wgmidx, csidx;
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

        timer_tick(avr, tmr, i, tmr->wgm_table[wgmidx], tmr->clock_src_table[csidx]);
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
