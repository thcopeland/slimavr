#ifndef AVR_H
#define AVR_H

#include <stdint.h>
#include "model.h"

#define EBADINST 1
#define EUNSUPPORTED 2
#define EBADADDR 3
#define EBADPADDR 4

#define SLEEPING 1

struct avr {
    struct avr_model model; // processor model
    uint8_t error;          // current error, if any
    uint8_t status;         // processor state
    uint8_t progress;       // cycles remaining for multi-cycle instructions
    uint32_t pc;            // program counter
    uint8_t *rom;           // program memory
    uint8_t *mem;           // sram and registers
};

struct avr *avr_init(struct avr_model model);
void avr_free(struct avr *avr);
void avr_step(struct avr *avr);

#endif
