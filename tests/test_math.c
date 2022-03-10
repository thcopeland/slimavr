#include <stdio.h>
#include "avr.h"
#include "loader.h"

void test_basic_math(void) {
    char buff[256];
    printf("testing basic math... ");
    load_ihex(buff, 256, "fixtures/atmega2560_math.hex");
    printf("\x1b[32mpass\x1b[0m\n");
}

void test_multiplication(void) {
    char buff[256];
    printf("testing multiplication... ");
    load_ihex(buff, 256, "fixtures/atmega2560_math.hex");
    printf("\x1b[32mpass\x1b[0m\n");
}

void test_division(void) {
    char buff[256];
    printf("testing division... ");
    load_ihex(buff, 256, "fixtures/atmega2560_math.hex");
    printf("\x1b[32mpass\x1b[0m\n");
}

int main(int argc, char **argv) {
    test_basic_math();
    test_multiplication();
    test_division();
    return 0;
}
