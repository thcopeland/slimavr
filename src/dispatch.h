#ifndef SLIMAVR_DISPATCH_H
#define SLIMAVR_DISPATCH_H

#include "avr.h"
#include "inst.h"

void avr_dispatch(struct avr *avr, uint16_t inst);

#endif
