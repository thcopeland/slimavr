#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_sram(void) {
    printf("testing SRAM... ");
    run_test("asm/sram.hex", 280, MODEL_ATMEGA1280);
}

void test_flash(void) {
    printf("testing flash... ");
    run_test("asm/flash.hex", 200, MODEL_ATMEGA1280);
}

void test_registers(void) {
    printf("testing registers... ");
    run_test("asm/register.hex", 100, MODEL_ATMEGA1280);
}

int main(int argc, char **argv) {
    // test_sram();
    // test_flash();
    // test_registers();
    return 0;
}
