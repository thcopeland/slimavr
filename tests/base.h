#include <stdio.h>
#include "slimavr.h"

void run_test(char *fname, long duration, struct avr_model model) {
    struct avr *avr = avr_init(model);
    FILE *f1 = fopen("../pb7.dat", "w"),
         *f2 = fopen("../tcnt0.dat", "w"),
         *f3 = fopen("../pg5.dat", "w");
    long c = 0;
    if (avr_load_ihex(avr, fname) == 0) {
        for (long i = 0; i < duration; i++) {
            avr_step(avr);

            // if (c > 0 || avr->reg[0x45]) c++;
            // fprintf(f1, "%lu\t%d\n", c, (avr->mem[0x25] > 0) * 80);
            // fprintf(f3, "%lu\t%d\n", c, (avr->mem[0x34] > 0) * 80);
            // fprintf(f2, "%lu\t%d\n", c, avr->mem[0x46]);

            if (avr->status == CPU_STATUS_CRASHED) {
                printf("cpu crashed (%d)\n", avr->error);
                break;
            }
        }

        if (avr->ram[0x42] != 0) {
            printf("fail (%d)\n", avr->ram[0x42]);
        } else {
            printf("pass\n");
        }
    }
    fclose(f1);
    fclose(f2);
    fclose(f3);
    avr_free(avr);
}
