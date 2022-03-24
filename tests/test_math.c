#include <stdio.h>
#include <assert.h>
#include "slimavr.h"

void test_arithmetic(void) {
    printf("testing basic math... ");
    struct avr *avr = avr_init(MODEL_ATMEGA1280);
    avr_load_ihex(avr, "fixtures/arithmetic.hex");
    for (int i = 0; i < 80; i++) avr_step(avr);
    assert(avr->mem[avr->model.ramstart+0x42] == 0);
    avr_free(avr);
    printf("pass\n");
}

int main(int argc, char **argv) {
    test_arithmetic();
    return 0;
}
