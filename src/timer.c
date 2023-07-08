#include "timer.h"
#include "timer_lut.h"
#include "utils.h"
#include "avr.h"

static void timerstate_reset(struct avr_timerstate *state) {
    state->wgm = WGM_RESERVED;
    state->top = 0;
    state->ovf = 0;
    state->sync = 0;
    state->prescale_mask = 0;
    state->reverse_counting = 0;
    state->idle = 1;
    state->coma_up_match = 0;
    state->coma_down_match = 0;
    state->coma_top_match = 0;
    state->coma_bottom_match = 0;
    state->comb_up_match = 0;
    state->comb_down_match = 0;
    state->comb_top_match = 0;
    state->comb_bottom_match = 0;
    state->comc_up_match = 0;
    state->comc_down_match = 0;
    state->comc_top_match = 0;
    state->comc_bottom_match = 0;
    state->prescale_clock = 0;
    state->matches_blocked = 0;
    state->counting_direction = 1;
    state->dirty = 0;
    state->ocra = 0;
    state->ocrb = 0;
    state->ocrc = 0;
    state->tmp = 0;
}

void avr_timers_reset(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        timerstate_reset(avr->timer_data+i);
    }
}

// must match avr_timer_cs order
static const uint16_t prescale_mask_table[] = {
    0, 0x000, 0x001, 0x003, 0x007, 0x00f, 0x01f, 0x03f, 0x07f, 0xff, 0x1ff, 0x3ff, 0, 0
};

static inline uint16_t get_timer_reg(struct avr *avr, const struct avr_timer *tmr, uint16_t reg) {
    return avr->reg[reg] | (tmr->resolution > 8 ? avr->reg[reg+1] << 8 : 0);
}

#define apply_com(reg, mask, com)   \
    (reg) = ((com) == NOP           \
        ? reg                       \
        : ((com) == SET             \
            ? (reg)|(mask)          \
            : ((com) == CLR         \
                ? (reg)&~(mask)     \
                : (reg)^(mask))))

// calculate and persist timer state
void avr_recompute_timer(struct avr *avr, const struct avr_timer *tmr, struct avr_timerstate *state) {
    uint8_t tccra = avr->reg[tmr->reg_tccra],
            tccrb = avr->reg[tmr->reg_tccrb];
    uint8_t wgmidx, csidx = tccrb & 0x07;

    // decode the control registers
    if (tmr->type == TIMER_REDUCED) {
        wgmidx = ((tccrb & 0x8) >> 1) | (tccra & 0x3);
    } else {
        wgmidx = ((tccrb & 0x18) >> 1) | (tccra & 0x3);
    }

    enum avr_timer_wgm wgm = tmr->wgm_table[wgmidx];
    uint8_t cs = tmr->clock_src_table[csidx];
    state->wgm = wgm;
    state->prescale_mask = prescale_mask_table[cs];
    state->idle = (wgm == WGM_RESERVED || cs == CS_DISABLED);

    const uint8_t *coma_table = empty_com_table,
                  *combc_table = empty_com_table;

    if (wgm == WGM_NORMAL || wgm == WGM_CLEAR_ON_COMPARE_ICR || wgm == WGM_CLEAR_ON_COMPARE_OCRA) {
        // sync immediately for these modes
        if (tmr->resolution > 8) {
            state->ocra = avr->reg[tmr->reg_ocra] | (avr->reg[tmr->reg_ocra+1] << 8);
            state->ocrb = avr->reg[tmr->reg_ocrb] | (avr->reg[tmr->reg_ocrb+1] << 8);
            state->ocrc = avr->reg[tmr->reg_ocrc] | (avr->reg[tmr->reg_ocrc+1] << 8);
        } else {
            state->ocra = avr->reg[tmr->reg_ocra];
            state->ocrb = avr->reg[tmr->reg_ocrb];
            state->ocrc = avr->reg[tmr->reg_ocrc];
        }
    } else {
        // need to update at the sync point
        state->dirty = 1;
    }

    switch (wgm) {
        case WGM_RESERVED:
            break;
        case WGM_NORMAL:
            state->top = (1 << tmr->resolution) - 1;
            state->ovf = state->top;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = non_pwm_com_table;
            combc_table = non_pwm_com_table;
            break;
        case WGM_CLEAR_ON_COMPARE_ICR:
            state->top = MAX(get_timer_reg(avr, tmr, tmr->reg_icr), 3);
            state->ovf = (1 << tmr->resolution) - 1;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = non_pwm_com_table;
            combc_table = non_pwm_com_table;
            break;
        case WGM_CLEAR_ON_COMPARE_OCRA:
            state->top = MAX(state->ocra, 3);
            state->ovf = (1 << tmr->resolution) - 1;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = non_pwm_com_table;
            combc_table = non_pwm_com_table;
            break;
        case WGM_FAST_PWM_8BIT:
            state->top = 0xff;
            state->ovf = state->top;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = fast_pwm_com_table1;
            combc_table = fast_pwm_com_table1;
            break;
        case WGM_FAST_PWM_9BIT:
            state->top = 0x1ff;
            state->ovf = state->top;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = fast_pwm_com_table1;
            combc_table = fast_pwm_com_table1;
            break;
        case WGM_FAST_PWM_10BIT:
            state->top = 0x3ff;
            state->ovf = state->top;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = fast_pwm_com_table1;
            combc_table = fast_pwm_com_table1;
            break;
        case WGM_FAST_PWM_ICR:
            state->top = MAX(get_timer_reg(avr, tmr, tmr->reg_icr), 3);
            state->ovf = state->top;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = fast_pwm_com_table2;
            combc_table = fast_pwm_com_table1;
            break;
        case WGM_FAST_PWM_OCRA:
            state->top = MAX(state->ocra, 3);
            state->ovf = state->top;
            state->sync = 0;
            state->reverse_counting = 0;
            coma_table = fast_pwm_com_table2;
            combc_table = fast_pwm_com_table1;
            break;
        case WGM_PHASE_PWM_8BIT:
            state->top = 0xff;
            state->ovf = 0;
            state->sync = state->top;
            state->reverse_counting = 1;
            coma_table = phase_freq_pwm_com_table1;
            combc_table = phase_freq_pwm_com_table1;
            break;
        case WGM_PHASE_PWM_9BIT:
            state->top = 0x1ff;
            state->ovf = 0;
            state->sync = state->top;
            state->reverse_counting = 1;
            coma_table = phase_freq_pwm_com_table1;
            combc_table = phase_freq_pwm_com_table1;
            break;
        case WGM_PHASE_PWM_10BIT:
            state->top = 0x3ff;
            state->ovf = 0;
            state->sync = state->top;
            state->reverse_counting = 1;
            coma_table = phase_freq_pwm_com_table1;
            combc_table = phase_freq_pwm_com_table1;
            break;
        case WGM_PHASE_PWM_ICR:
            state->top = MAX(get_timer_reg(avr, tmr, tmr->reg_icr), 3);
            state->ovf = 0;
            state->sync = state->top;
            state->reverse_counting = 1;
            coma_table = phase_freq_pwm_com_table1;
            combc_table = phase_freq_pwm_com_table1;
            break;
        case WGM_PHASE_PWM_OCRA:
            state->top = MAX(state->ocra, 3);
            state->ovf = 0;
            state->sync = state->top;
            state->reverse_counting = 1;
            coma_table = phase_freq_pwm_com_table2;
            combc_table = phase_freq_pwm_com_table1;
            break;
        case WGM_PHASE_FREQ_PWM_ICR:
            state->top = MAX(get_timer_reg(avr, tmr, tmr->reg_icr), 3);
            state->ovf = 0;
            state->sync = 0;
            state->reverse_counting = 1;
            coma_table = phase_freq_pwm_com_table1;
            combc_table = phase_freq_pwm_com_table1;
            break;
        case WGM_PHASE_FREQ_PWM_OCRA:
            state->top = MAX(state->ocra, 3);
            state->ovf = 0;
            state->sync = 0;
            state->reverse_counting = 1;
            coma_table = phase_freq_pwm_com_table2;
            combc_table = phase_freq_pwm_com_table1;
            break;
    }

    // handle pin outputs
    uint8_t coma = tccra >> 6,
            comb = (tmr->comparators > 1 ? (tccra>>4) & 0x03 : 0),
            comc = (tmr->comparators > 2 ? (tccra>>2) & 0x03 : 0);

    uint8_t base = 32*(state->ocra == 0) + 16*(state->ocra == state->top) + 4*coma;
    state->coma_up_match = coma_table[base + 0];
    state->coma_down_match = coma_table[base + 1];
    state->coma_top_match = coma_table[base + 2];
    state->coma_bottom_match = coma_table[base + 3];

    base = 32*(state->ocrb == 0) + 16*(state->ocrb == state->top) + 4*comb;
    state->comb_up_match = combc_table[base + 0];
    state->comb_down_match = combc_table[base + 1];
    state->comb_top_match = combc_table[base + 2];
    state->comb_bottom_match = combc_table[base + 3];

    base = 32*(state->ocrc == 0) + 16*(state->ocrc == state->top) + 4*comc;
    state->comc_up_match = combc_table[base + 0];
    state->comc_down_match = combc_table[base + 1];
    state->comc_top_match = combc_table[base + 2];
    state->comc_bottom_match = combc_table[base + 3];
}

static void timer_tick(struct avr *avr, const struct avr_timer *tmr, struct avr_timerstate *state) {
    if (state->idle) return;
    // handle prescale
    if ((++state->prescale_clock) & state->prescale_mask) return;

    // synchronize double-buffered registers
    uint16_t clk = avr->reg[tmr->reg_tcnt] | (tmr->resolution > 8 ? avr->reg[tmr->reg_tcnt+1] << 8 : 0);
    if (clk == state->sync) {
        if (tmr->resolution > 8) {
            state->ocra = avr->reg[tmr->reg_ocra] | (avr->reg[tmr->reg_ocra+1] << 8);
            state->ocrb = avr->reg[tmr->reg_ocrb] | (avr->reg[tmr->reg_ocrb+1] << 8);
            state->ocrc = avr->reg[tmr->reg_ocrc] | (avr->reg[tmr->reg_ocrc+1] << 8);
        } else {
            state->ocra = avr->reg[tmr->reg_ocra];
            state->ocrb = avr->reg[tmr->reg_ocrb];
            state->ocrc = avr->reg[tmr->reg_ocrc];
        }

        // need to recompute with the newly-synced OCRnA value
        if (state->dirty) {
            avr_recompute_timer(avr, tmr, state);
            state->dirty = 0;
        }
    }

    // handle the six significant clock values: OVF, TOP, BOTTOM, OCRnA, OCRnB, OCRnC

    if (clk == state->ovf) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_tovf;
    }

    if (clk == state->top) {
        if (state->reverse_counting) state->counting_direction = -1;
        apply_com(avr->reg[tmr->reg_oca], tmr->msk_oca, state->coma_top_match);
        if (tmr->comparators > 1) {
            apply_com(avr->reg[tmr->reg_ocb], tmr->msk_ocb, state->comb_top_match);
            if (tmr->comparators > 2) {
                apply_com(avr->reg[tmr->reg_occ], tmr->msk_occ, state->comc_top_match);
            }
        }
    } else if (clk == 0) {
        state->counting_direction = 1;
        apply_com(avr->reg[tmr->reg_oca], tmr->msk_oca, state->coma_bottom_match);
        if (tmr->comparators > 1) {
            apply_com(avr->reg[tmr->reg_ocb], tmr->msk_ocb, state->comb_bottom_match);
            if (tmr->comparators > 2) {
                apply_com(avr->reg[tmr->reg_occ], tmr->msk_occ, state->comc_bottom_match);
            }
        }
    }

    if (clk == state->ocra) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_ocfa;

        if (state->counting_direction > 0) {
            apply_com(avr->reg[tmr->reg_oca], tmr->msk_oca, state->coma_up_match);
        } else {
            apply_com(avr->reg[tmr->reg_oca], tmr->msk_oca, state->coma_down_match);
        }
    }

    if (clk == state->ocrb && tmr->comparators > 1) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_ocfb;

        if (state->counting_direction > 0) {
            apply_com(avr->reg[tmr->reg_ocb], tmr->msk_ocb, state->comb_up_match);
        } else {
            apply_com(avr->reg[tmr->reg_ocb], tmr->msk_ocb, state->comb_down_match);
        }
    }

    if (clk == state->ocrc && tmr->comparators > 2) {
        avr->reg[tmr->reg_tifr] |= tmr->msk_ocfc;

        if (state->counting_direction > 0) {
            apply_com(avr->reg[tmr->reg_occ], tmr->msk_occ, state->comc_up_match);
        } else {
            apply_com(avr->reg[tmr->reg_occ], tmr->msk_occ, state->comc_down_match);
        }
    }

    clk += state->counting_direction;
    if (clk > state->top) clk = 0;

    avr->reg[tmr->reg_tcnt] = clk;
    if (tmr->resolution > 8) {
        avr->reg[tmr->reg_tcnt+1] = clk >> 8;
    }
}

void avr_update_timers(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        timer_tick(avr, avr->model.timers+i, avr->timer_data+i);
    }
}

uint32_t avr_find_timer_interrupt(struct avr *avr) {
    for (int i = 0; i < avr->model.timer_count; i++) {
        const struct avr_timer *tmr = avr->model.timers+i;
        // TODO check sleep, any special stuff
        uint8_t tifr = avr->reg[tmr->reg_tifr],
                timsk = avr->reg[tmr->reg_timsk];

        if ((tifr | timsk) == 0) continue;

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
