#include <assert.h>
#include "avr.h"
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
        avr_panic(avr, AVR_INVALID_STACK_ACCESS);
    }
}

uint8_t sim_pop(struct avr *avr) {
    uint16_t sp = get_sp(avr);

    if (sp < avr->model.ramstart+avr->model.ramsize) {
        set_sp(avr, ++sp);
        return avr->mem[sp];
    } else {
        avr_panic(avr, AVR_INVALID_STACK_ACCESS);
        return 0;
    }
}


// map between register type and index
#define reg_type_timer(type) (((int)type-REG_TIMER0_HIGH)/3)
#define reg_type_port_in(type) ((int)type-REG_PORTA_IN)

// ensure that the mapping remains correct
static_assert(reg_type_timer(REG_TIMER0_HIGH) == 0, "reg_type_timer and avr_register_type mismatch for REG_TIMER0_HIGH");
static_assert(reg_type_timer(REG_TIMER0_LOW) == 0, "reg_type_timer and avr_register_type mismatch for REG_TIMER0_LOW");
static_assert(reg_type_timer(REG_TIMER0_CTRL) == 0, "reg_type_timer and avr_register_type mismatch for REG_TIMER0_CTRL");
static_assert(reg_type_timer(REG_TIMER1_HIGH) == 1, "reg_type_timer and avr_register_type mismatch for REG_TIMER1_HIGH");
static_assert(reg_type_timer(REG_TIMER1_LOW) == 1, "reg_type_timer and avr_register_type mismatch for REG_TIMER1_LOW");
static_assert(reg_type_timer(REG_TIMER1_CTRL) == 1, "reg_type_timer and avr_register_type mismatch for REG_TIMER1_CTRL");
static_assert(reg_type_timer(REG_TIMER2_HIGH) == 2, "reg_type_timer and avr_register_type mismatch for REG_TIMER2_HIGH");
static_assert(reg_type_timer(REG_TIMER2_LOW) == 2, "reg_type_timer and avr_register_type mismatch for REG_TIMER2_LOW");
static_assert(reg_type_timer(REG_TIMER2_CTRL) == 2, "reg_type_timer and avr_register_type mismatch for REG_TIMER2_CTRL");
static_assert(reg_type_timer(REG_TIMER3_HIGH) == 3, "reg_type_timer and avr_register_type mismatch for REG_TIMER3_HIGH");
static_assert(reg_type_timer(REG_TIMER3_LOW) == 3, "reg_type_timer and avr_register_type mismatch for REG_TIMER3_LOW");
static_assert(reg_type_timer(REG_TIMER3_CTRL) == 3, "reg_type_timer and avr_register_type mismatch for REG_TIMER3_CTRL");
static_assert(reg_type_timer(REG_TIMER4_HIGH) == 4, "reg_type_timer and avr_register_type mismatch for REG_TIMER4_HIGH");
static_assert(reg_type_timer(REG_TIMER4_LOW) == 4, "reg_type_timer and avr_register_type mismatch for REG_TIMER4_LOW");
static_assert(reg_type_timer(REG_TIMER4_CTRL) == 4, "reg_type_timer and avr_register_type mismatch for REG_TIMER4_CTRL");
static_assert(reg_type_timer(REG_TIMER5_HIGH) == 5, "reg_type_timer and avr_register_type mismatch for REG_TIMER5_HIGH");
static_assert(reg_type_timer(REG_TIMER5_LOW) == 5, "reg_type_timer and avr_register_type mismatch for REG_TIMER5_LOW");
static_assert(reg_type_timer(REG_TIMER5_CTRL) == 5, "reg_type_timer and avr_register_type mismatch for REG_TIMER5_CTRL");
static_assert(reg_type_port_in(REG_PORTA_IN) == 0, "reg_type_port_in and avr_register_type mismatch for REG_PORTA_IN");
static_assert(reg_type_port_in(REG_PORTB_IN) == 1, "reg_type_port_in and avr_register_type mismatch for REG_PORTB_IN");
static_assert(reg_type_port_in(REG_PORTC_IN) == 2, "reg_type_port_in and avr_register_type mismatch for REG_PORTC_IN");
static_assert(reg_type_port_in(REG_PORTD_IN) == 3, "reg_type_port_in and avr_register_type mismatch for REG_PORTD_IN");
static_assert(reg_type_port_in(REG_PORTE_IN) == 4, "reg_type_port_in and avr_register_type mismatch for REG_PORTE_IN");
static_assert(reg_type_port_in(REG_PORTF_IN) == 5, "reg_type_port_in and avr_register_type mismatch for REG_PORTF_IN");
static_assert(reg_type_port_in(REG_PORTG_IN) == 6, "reg_type_port_in and avr_register_type mismatch for REG_PORTG_IN");
static_assert(reg_type_port_in(REG_PORTH_IN) == 7, "reg_type_port_in and avr_register_type mismatch for REG_PORTH_IN");
static_assert(reg_type_port_in(REG_PORTI_IN) == 8, "reg_type_port_in and avr_register_type mismatch for REG_PORTI_IN");
static_assert(reg_type_port_in(REG_PORTJ_IN) == 9, "reg_type_port_in and avr_register_type mismatch for REG_PORTJ_IN");
static_assert(reg_type_port_in(REG_PORTK_IN) == 10, "reg_type_port_in and avr_register_type mismatch for REG_PORTK_IN");
static_assert(reg_type_port_in(REG_PORTL_IN) == 11, "reg_type_port_in and avr_register_type mismatch for REG_PORTL_IN");

uint8_t avr_get_reg(struct avr *avr, uint16_t reg) {
    enum avr_register_type type = avr->model.regmap[reg].type;

    switch (type) {
        case REG_RESERVED:
            return 0xff;

        case REG_VALUE:
        case REG_UNSUPPORTED:
        case REG_CLEAR_ON_SET:
        case REG_EEP_CONTROL:
        case REG_SPM_CONTROL:
        case REG_TIMER0_CTRL:
        case REG_TIMER1_CTRL:
        case REG_TIMER2_CTRL:
        case REG_TIMER3_CTRL:
        case REG_TIMER4_CTRL:
        case REG_TIMER5_CTRL:
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

        case REG_PORTA_IN:
        case REG_PORTB_IN:
        case REG_PORTC_IN:
        case REG_PORTD_IN:
        case REG_PORTE_IN:
        case REG_PORTF_IN:
        case REG_PORTG_IN:
        case REG_PORTH_IN:
        case REG_PORTI_IN:
        case REG_PORTJ_IN:
        case REG_PORTK_IN:
        case REG_PORTL_IN:
            return avr_io_read_port(avr, 'A' + (char) reg_type_port_in(type));

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
        case REG_PORTA_IN:
        case REG_PORTB_IN:
        case REG_PORTC_IN:
        case REG_PORTD_IN:
        case REG_PORTE_IN:
        case REG_PORTF_IN:
        case REG_PORTG_IN:
        case REG_PORTH_IN:
        case REG_PORTI_IN:
        case REG_PORTJ_IN:
        case REG_PORTK_IN:
        case REG_PORTL_IN:
            avr->reg[reg] = val;
            break;

        case REG_CLEAR_ON_SET:
            avr->reg[reg] &= ~val;
            break;

        case REG_EEP_CONTROL:
            avr_set_eeprom_reg(avr, reg, val, 0xff);
            break;

        case REG_SPM_CONTROL:
            avr_set_flash_reg(avr, reg, val, 0xff);
            break;

        case REG_TIMER0_CTRL:
        case REG_TIMER1_CTRL:
        case REG_TIMER2_CTRL:
        case REG_TIMER3_CTRL:
        case REG_TIMER4_CTRL:
        case REG_TIMER5_CTRL:
            avr->reg[reg] = val;
            avr_recompute_timer(avr, avr->model.timers + reg_type_timer(type), avr->timer_data + reg_type_timer(type));
            break;

        case REG_TIMER0_LOW:
        case REG_TIMER1_LOW:
        case REG_TIMER2_LOW:
        case REG_TIMER3_LOW:
        case REG_TIMER4_LOW:
        case REG_TIMER5_LOW:
            avr->reg[reg+1] = avr->timer_data[reg_type_timer(type)].tmp;
            avr->reg[reg] = val;
            avr_recompute_timer(avr, avr->model.timers + reg_type_timer(type), avr->timer_data + reg_type_timer(type));
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

void avr_set_reg_bits(struct avr *avr, uint16_t reg, uint8_t val, uint8_t mask) {
    enum avr_register_type type = avr->model.regmap[reg].type;

    switch (type) {
        case REG_EEP_CONTROL:
            avr_set_eeprom_reg(avr, reg, val, mask);
            break;

        case REG_SPM_CONTROL:
            avr_set_flash_reg(avr, reg, val, mask);
            break;

        default:
            avr_set_reg(avr, reg, (avr->mem[reg] & ~mask) | (val & mask));
    }
}
