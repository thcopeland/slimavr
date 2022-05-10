#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

struct avr_model {
    uint32_t romsize;       // size of program memory in bytes
    uint32_t memsize;       // size of ram, NOT including memory-mapped registers, in bytes
    uint16_t ramstart;      // starting address of general sram
    uint8_t pcsize;         // size of the program counter in bytes
    uint8_t in_out_offset;  // offset when accessing memory-mappped registers with IN/OUT

    // memory-mapped registers
    uint16_t reg_rampx;
    uint16_t reg_rampy;
    uint16_t reg_rampz;
    uint16_t reg_eind;
    uint16_t reg_stack;
    uint16_t reg_status;
    // todo
};

extern const struct avr_model MODEL_ATMEGA1280;
extern const struct avr_model MODEL_ATMEGA2560;


#endif
