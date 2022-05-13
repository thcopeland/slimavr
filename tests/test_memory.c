#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_sram(void) {
    printf("testing SRAM... ");
    run_test("asm/sram.hex", 280, MODEL_ATMEGA1280);
}

void test_flash(void) {
    printf("testing SRAM... ");
    run_test("asm/flash.hex", 180, MODEL_ATMEGA1280);
}

int main(int argc, char **argv) {
    test_sram();
    test_flash();
    return 0;
}
