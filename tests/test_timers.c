#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_8bit_timer_interrupts(void) {
    printf("testing 8 bit timers (part 1)... ");
    run_test("asm/timer_8bit_interrupts.hex", 7400, MODEL_ATMEGA1280);
}

void test_8bit_timer_output(void) {
    printf("testing 8 bit timers (part 2)... ");
    run_test("asm/timer_8bit_output.hex", 1450, MODEL_ATMEGA1280);
}


void test_16bit_timer_interrupts(void) {
    printf("testing 16 bit timers (part 1)... ");
    run_test("asm/timer_16bit_interrupts.hex", 800000, MODEL_ATMEGA1280);
}

void test_16bit_timer_output(void) {
    printf("testing 16 bit timers (part 2)... ");
    run_test("asm/timer_16bit_output.hex", 120000, MODEL_ATMEGA1280);
}

int main(int argc, char **argv) {
    test_8bit_timer_interrupts();
    test_8bit_timer_output();
    test_16bit_timer_interrupts();
    test_16bit_timer_output();
    return 0;
}
