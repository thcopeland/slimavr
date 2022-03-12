#include <stdio.h>
#include "avr.h"
#include "loader.h"

void test_basic_math(void) {
    printf("testing basic math... ");
    struct avr *avr = avr_init(0);
    avr_load_ihex(avr, "fixtures/atmega2560_math.hex");
    avr_free(avr);
    printf("\x1b[32mpass\x1b[0m\n");
}

int main(int argc, char **argv) {
    test_basic_math();
    return 0;
}
