#ifndef LOADER_H
#define LOADER_H

#include "avr.h"

#define EFILE 1
#define EFORMAT 2
#define ECHECKSUM 3

int avr_load_ihex(struct avr *avr, char *fname);

#endif
