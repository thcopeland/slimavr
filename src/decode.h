#ifndef SLIMAVR_DECODE_H
#define SLIMAVR_DECODE_H

#include <stdint.h>

void avr_decode(char *str, int size, uint16_t inst, uint16_t inst2);

#endif
