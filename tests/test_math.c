#include <stdio.h>
#include <assert.h>
#include "slimavr.h"

void test_arithmetic(void) {
    printf("testing basic math... ");
    struct avr *avr = avr_init(MODEL_ATMEGA1280);
    avr_load_ihex(avr, "fixtures/arithmetic.hex");
    for (int i = 0; i < 160; i++) avr_step(avr);
    if (avr->mem[avr->model.ramstart+0x42] != 0) {
        printf("fail (%d)\n", avr->mem[avr->model.ramstart+0x42]);
    } else {
        printf("pass\n");
    }
    avr_free(avr);
}

int main(int argc, char **argv) {
    test_arithmetic();
    return 0;
}
