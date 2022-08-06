#include <stdio.h>
#include "src/slimavr.h"

int main() {
    struct avr *avr = avr_init(AVR_MODEL_ATMEGA1280);
    long duration = 60;
    if (avr_load_ihex(avr, "debug.S.hex") == 0) {
        for (long i = 0; i < duration; i++) {
            avr_step(avr);

            if (avr->status == CPU_STATUS_CRASHED) {
                printf("cpu crashed (%d)\n", avr->error);
                break;
            }
        }

        printf("r16 = %d\n", avr->reg[16]);
        printf("r17 = %d\n", avr->reg[17]);

        if (avr->ram[0x42] != 0) {
            printf("fail (%d)\n", avr->ram[0x42]);
        } else {
            printf("pass\n");
        }
    }
    avr_free(avr);
}
