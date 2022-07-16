#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_jmp(void) {
    printf("testing jumps... ");
    run_test("asm/jmp.hex", 40, MODEL_ATMEGA2560);
}

void test_call_2b(void) {
    printf("testing calls (2 byte PC)... ");
    run_test("asm/call_2b.hex", 140, MODEL_ATMEGA1280);
}

void test_call_3b(void) {
    printf("testing calls (3 byte PC)... ");
    run_test("asm/call_3b.hex", 210, MODEL_ATMEGA2560);
}

int main(int argc, char **argv) {
    test_jmp();
    test_call_2b();
    test_call_3b();
    return 0;
}
