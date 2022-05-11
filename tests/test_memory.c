#include <stdio.h>
#include "slimavr.h"
#include "base.h"

void test_sram(void) {
    printf("testing SRAM... ");
    run_test("asm/sram.hex", 260, MODEL_ATMEGA1280);
}

int main(int argc, char **argv) {
    test_sram();
    return 0;
}
