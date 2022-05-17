#include "interrupt.h"
#include "model.h"

uint8_t avr_get_reg(struct avr *avr, uint16_t reg) {
    return avr->reg[reg];
}

int avr_set_reg(struct avr *avr, uint16_t reg, uint8_t val) {
    avr->reg[reg] = val;
    return 0;
}

void avr_check_interrupts(struct avr *avr);
