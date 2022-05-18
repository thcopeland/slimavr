#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "avr.h"

void sim_push(struct avr *avr, uint8_t val);

uint8_t sim_pop(struct avr *avr);

#endif
