#include <stdio.h>
#include "slimavr.h"

void run_test(char *fname, long duration, struct avr_model model) {
    struct avr *avr = avr_init(model);
    if (avr_load_ihex(avr, fname) == 0) {
        for (long i = 0; i < duration; i++) {
            avr_step(avr);

            if (avr->status == CPU_STATUS_CRASHED) {
                printf("cpu crashed (%d)\n", avr->error);
                break;
            }
        }

        printf("r16 = %d\n", avr->reg[16]);
        printf("r17 = %d\n", avr->reg[17]);
        printf("r18 = %d\n", avr->reg[18]);
        printf("r20 = %d\n", avr->reg[20]);

        if (avr->ram[0x42] != 0) {
            printf("fail (%d)\n", avr->ram[0x42]);
        } else {
            printf("pass\n");
        }
    }
    avr_free(avr);
}
