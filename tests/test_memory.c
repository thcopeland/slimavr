#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_sram(void) {
    printf("testing SRAM... ");
    run_test("asm/sram.hex", 300, AVR_MODEL_ATMEGA1280);
}

void test_flash(void) {
    printf("testing flash... ");
    run_test("asm/flash.hex", 200000, AVR_MODEL_ATMEGA1280);
}

void test_eeprom(void) {
    printf("testing EEPROM...");
    run_test("asm/eeprom.hex", 250000, AVR_MODEL_ATMEGA1280);
}

void test_registers(void) {
    printf("testing registers... ");
    run_test("asm/register.hex", 100, AVR_MODEL_ATMEGA1280);
}

int main(int argc, char **argv) {
    test_sram();
    test_flash();
    test_eeprom();
    test_registers();
    return 0;
}
