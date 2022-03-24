#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

struct avr_model {
    uint32_t romsize;       // size of program memory in bytes
    uint32_t memsize;       // size of ram, NOT including memory-mapped registers, in bytes
    uint16_t ramstart;      // starting address of general sram
    uint8_t pcsize;         // size of the program counter in bytes

    // memory-mapped registers
    uint8_t in_out_offset;
    uint16_t status_reg;
    uint16_t stack_reg;
    // todo
};

extern const struct avr_model MODEL_ATMEGA1280;

#endif
