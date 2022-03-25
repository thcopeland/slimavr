#include <stdio.h>
#include "slimavr.h"

void run_test(char *fname, long duration, struct avr_model model) {
    struct avr *avr = avr_init(model);
    if (avr_load_ihex(avr, fname) == 0) {
        for (long i = 0; i < duration; i++) {
            avr_step(avr);
        }
        if (avr->mem[avr->model.ramstart+0x42] != 0) {
            printf("fail (%d)\n", avr->mem[avr->model.ramstart+0x42]);
        } else {
            printf("pass\n");
        }
    }
    avr_free(avr);
}
