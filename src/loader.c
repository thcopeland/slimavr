#include <stdio.h>
#include "loader.h"

int load_ihex(char *src, int size, char *fname) {
    FILE *f = fopen(fname, "r");

    if (!f) return ENOFILE;

    return 0;
}
