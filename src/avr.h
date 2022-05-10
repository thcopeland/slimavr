#ifndef AVR_H
#define AVR_H

#include <stdint.h>
#include "model.h"

#define CPU_INVALID_INSTRUCTION 1
#define CPU_UNSUPPORTED_INSTRUCTION 2
#define CPU_INVALID_RAM_ADDRESS 3
#define CPU_INVALID_ROM_ADDRESS 4
#define CPU_INVALID_STACK_ACCESS 5

#define CPU_STATUS_NORMAL 0
#define CPU_STATUS_CRASHED 1
#define CPU_STATUS_COMPLETING 2
#define CPU_STATUS_SLEEPING 3
#define CPU_STATUS_INTERRUPTING 4

struct avr {
    struct avr_model model; // processor model
    uint8_t error;          // current error, if any
    uint8_t status;         // processor state
    int8_t progress;        // cycles remaining for multi-cycle instructions
    uint32_t pc;            // program counter

    // memory
    uint8_t *mem;

    // memory segments
    uint8_t *rom;           // program memory
    uint8_t *reg;           // registers
    uint8_t *ram;           // sram
    uint8_t *eep;           // eeprom
};

struct avr *avr_init(struct avr_model model);
void avr_free(struct avr *avr);
void avr_step(struct avr *avr);

#endif
