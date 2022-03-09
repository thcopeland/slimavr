#ifndef LOADER_H
#define LOADER_H

#define ENOFILE 1
#define EBADOFFSET 2

#include "avr.h"

int load_ihex(char *src, int size, char *fname);

#endif
