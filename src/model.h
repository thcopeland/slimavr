#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

struct avr_model {
    // available storage media
    uint32_t romsize;       // size of program memory in bytes
    uint32_t regsize;       // size of register file in bytes
    uint32_t ramsize;       // size of sram in bytes
    uint32_t eepsize;       // size of eeprom in bytse

    // Most supported models have only two segments (register file and sram)
    // within the address space, but we allow mapping flash and eeprom for
    // potential future compatibility.
    //
    // Typical layout (some segments might not be mapped, or be in different orders):
    //
    // *--------------------------------------*    0x0000
    // | Register file (always first)         |
    // *--------------------------------------*    eepstart
    // | EEPROM (might not be mapped)         |
    // *--------------------------------------*    ramstart
    // | SRAM                                 |
    // *--------------------------------------*    progstart
    // | Program Memory (might not be mapped) |
    // *--------------------------------------*    memend
    uint32_t ramstart;      // first address pointing to actual sram
    uint32_t romstart;      // start of mapped program memory (0 for unmapped)
    uint32_t eepstart;      // start of mapped eeprom (0 for unmapped)
    uint32_t memend;        // end of the data address space

    // general model information
    uint8_t pcsize;         // size of the program counter in bytes
    uint8_t in_out_offset;  // offset when accessing memory-mappped registers with IN/OUT

    // memory-mapped register addresses
    uint16_t reg_rampz;
    uint16_t reg_eind;
    uint16_t reg_stack;
    uint16_t reg_status;
    // todo
};

extern const struct avr_model MODEL_ATMEGA1280;
extern const struct avr_model MODEL_ATMEGA2560;


#endif
