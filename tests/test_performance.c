#include <stdio.h>
#include <time.h>
#include "slimavr.h"

void test_performance(void) {
    printf("testing performance... ");
    struct avr *avr = avr_init(AVR_MODEL_ATMEGA1280);
    if (avr_load_ihex(avr, "asm/benchmark.hex") == 0) {
        clock_t start = clock();
        while(avr->ram[0x42] == 0) {
            avr_step(avr);

            if (avr->status == MCU_STATUS_CRASHED) {
                printf("cpu crashed (%d)\n", avr->error);
                break;
            }
        }
        double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
        printf("%.3f MHz\n", (double) avr->clock/elapsed/1000000);
    }
    avr_free(avr);
}

int main(int argc, char **argv) {
    test_performance();
    return 0;
}
