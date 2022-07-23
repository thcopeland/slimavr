#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_arithmetic(void) {
    printf("testing arithmetic... ");
    run_test("asm/arithmetic.hex", 180, MODEL_ATMEGA1280);
}

void test_bitwise(void) {
    printf("testing bitwise operations... ");
    run_test("asm/bitwise.hex", 340, MODEL_ATMEGA1280);
}

void test_multiplication(void) {
    printf("testing multiplication operations... ");
    run_test("asm/multiplication.hex", 130, MODEL_ATMEGA1280);
}

int main(int argc, char **argv) {
    // test_arithmetic();
    // test_bitwise();
    // test_multiplication();
    return 0;
}
