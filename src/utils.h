#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

struct avr;

#define MAX(a,b) ({             \
    typeof (a) _a = (a);        \
    typeof (b) _b = (b);        \
    _a > _b ? _a : _b;          \
})

#define MIN(a,b) ({             \
    typeof (a) _a = (a);        \
    typeof (b) _b = (b);        \
    _a < _b ? _a : _b;          \
})

void sim_push(struct avr *avr, uint8_t val);
uint8_t sim_pop(struct avr *avr);

uint8_t avr_get_reg(struct avr *avr, uint16_t reg);
void avr_set_reg(struct avr *avr, uint16_t reg, uint8_t val);
void avr_set_reg_bits(struct avr *avr, uint16_t reg, uint8_t val, uint8_t mask);

#endif
