#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_8bit_timer_simple(void) {
    printf("testing 8 bit timers (part 1)... ");
    run_test("asm/timer_8bit_simple.hex", 7400, MODEL_ATMEGA1280);
}

// void test_8bit_timer_complex(void) {
//     printf("testing 8 bit timers (part 2)... ");
//     run_test("asm/timer_8bit_complex.hex", 180, MODEL_ATMEGA1280);
// }
//
// void test_16bit_timer_pwm(void) {
//     printf("testing 8 bit timers (part 3)... ");
//     run_test("asm/timer_8bit_pwm.hex", 180, MODEL_ATMEGA1280);
// }


int main(int argc, char **argv) {
    // test_8bit_timer_simple();
    // test_8bit_timer_complex();
    // test_8bit_timer_pwm();
    return 0;
}
