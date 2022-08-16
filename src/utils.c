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


// map between register type and timer index
#define reg_type_timer(type) (((int)type-6)/2)

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
        case REG_SPM_CONTROL:
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
            avr_set_eeprom_reg(avr, reg, val, 0xff);
            break;

        case REG_SPM_CONTROL:
            avr_set_flash_reg(avr, reg, val, 0xff);
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
