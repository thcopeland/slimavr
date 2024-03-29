#include <stdio.h>
#include "slimavr.h"

void run_test(char *fname, long duration, struct avr_model model) {
    struct avr *avr = avr_new(model);
    // FILE *f1 = fopen("../ch1.dat", "w"),
    //      *f2 = fopen("../tcnt.dat", "w"),
    //      *f3 = fopen("../ch2.dat", "w");
    // long c = 0;
    if (avr_load_ihex(avr, fname) == 0) {
        for (long i = 0; i < duration; i++) {
            avr_step(avr);

            // if (c > 0 || avr->reg[0x45]) c++;
            // fprintf(f1, "%lu\t%d\n", c, ((avr->mem[0x25] & 0x80) > 0) * 25);
            // fprintf(f3, "%lu\t%d\n", c, ((avr->mem[0x34] & 0x20) > 0) * 30);
            // fprintf(f2, "%lu\t%d\n", c, avr->mem[0x46]);

            if (avr->status == MCU_STATUS_CRASHED) {
                avr_dump(avr, NULL);
                break;
            }
        }

        if (avr->ram[0x42] != 0) {
            printf("fail (%d)\n", avr->ram[0x42]);
        } else {
            printf("pass\n");
        }
    }
    // fclose(f1);
    // fclose(f2);
    // fclose(f3);
    avr_free(avr);
}
