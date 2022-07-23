#ifndef DISPATCH_H
#define DISPATCH_H

#include "avr.h"
#include "inst.h"

void avr_dispatch(struct avr *avr, uint8_t inst_l, uint8_t inst_h);

#endif