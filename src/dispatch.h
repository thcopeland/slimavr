#ifndef DISPATCH_H
#define DISPATCH_H

#include "avr.h"
#include "inst.h"

void avr_dispatch(struct avr *avr, uint16_t inst);

#endif
