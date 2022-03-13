#include <stdio.h>
#include "slimavr.h"

void test_basic_math(void) {
    printf("testing basic math... ");
    struct avr *avr = avr_init(MODEL_ATMEGA1280);
    avr_load_ihex(avr, "fixtures/atmega2560_math.hex");

    for(int i = 0; i < 80; i++) {
        avr_step(avr);
    }

    avr_free(avr);
    printf("pass\n");
}

int main(int argc, char **argv) {
    test_basic_math();
    return 0;
}
