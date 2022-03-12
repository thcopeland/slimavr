#include <stdlib.h>
#include "avr.h"

struct avr *avr_init(int model) {
    struct avr *avr = malloc(sizeof(*avr));
    if (avr) {
        avr->error = 0;
        avr->status = 0;
        avr->model = model;
        avr->pc = 0;
        avr->clock = 0;
        avr->rom = malloc(128*1024);
        avr->mem = malloc(4*1024);
        if (!avr->rom || !avr->mem) {
            free(avr->rom);
            free(avr->mem);
            free(avr);
            avr = NULL;
        }
    }
    return avr;
}

void avr_step(struct avr *avr) {

}

void avr_free(struct avr *avr) {
    if (avr) {
        free(avr->rom);
        free(avr->mem);
        free(avr);
    }
}
