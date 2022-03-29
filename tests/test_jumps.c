#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_jmp(void) {
    printf("testing jumps... ");
    run_test("asm/jmp.hex", 40, MODEL_ATMEGA2560);
}

void test_call(void) {
    printf("testing calls... ");
    run_test("asm/call.hex", 20, MODEL_ATMEGA2560);
}

int main(int argc, char **argv) {
    test_jmp();
    // test_call();
    return 0;
}
