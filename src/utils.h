#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

struct avr;

void sim_push(struct avr *avr, uint8_t val);
uint8_t sim_pop(struct avr *avr);

uint8_t avr_get_reg(struct avr *avr, uint16_t reg);
void avr_set_reg(struct avr *avr, uint16_t reg, uint8_t val);
void avr_set_reg_bits(struct avr *avr, uint16_t reg, uint8_t val, uint8_t mask);

#endif
