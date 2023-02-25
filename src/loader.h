#ifndef SLIMAVR_LOADER_H
#define SLIMAVR_LOADER_H

#include "avr.h"

#define AVR_EFILE 1
#define AVR_EFORMAT 2
#define AVR_ECHECKSUM 3

int avr_load_ihex(struct avr *avr, char *fname);

#endif
