#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>
#include "port.h"
#include "timer.h"

enum avr_register_type {
    REG_VALUE,              // register is accessed directly
    REG_RESERVED,           // always read 0xff
    REG_UNSUPPORTED,        // same as REG_VALUE
    REG_CLEAR_ON_SET,       // a bit is cleared when 1 is written
    REG_ATOMIC_HIGH,        // atomic 16-bit access, high byte
    REG_ATOMIC_LOW,         // atomic 16-bit access, low byte, triggers the 16-bit operation
    REG_TIMER0_HIGH_BUFF,   // atomic 16-bit access for high byte but to an internal 16-bit buffer
    REG_TIMER0_LOW_BUFF,    // atomic 16-bit access for low byte, triggers the 16-bit operation (use this for 8-bit too, eg OCRA0)
    REG_TIMER1_HIGH_BUFF,
    REG_TIMER1_LOW_BUFF,
    REG_TIMER2_HIGH_BUFF,
    REG_TIMER2_LOW_BUFF,
    REG_TIMER3_HIGH_BUFF,
    REG_TIMER3_LOW_BUFF,
    REG_TIMER4_HIGH_BUFF,
    REG_TIMER4_LOW_BUFF,
    REG_TIMER5_HIGH_BUFF,
    REG_TIMER5_LOW_BUFF

    // at this time, more complex behavior such as SPI, USART, external memory,
    // external clocking, etc is not supported.
};

struct avr_register {
    enum avr_register_type type;
};

struct avr_model {
    // general model information
    uint8_t pcsize;             // size of the program counter in bytes
    uint8_t io_offset;          // offset when accessing memory-mappped registers with IN/OUT
    uint8_t interrupt_time;     // how long an interrupt take to occur
    uint64_t flags;             // special behavior flags
    uint16_t eep_pgsize;        // eeprom page size in bytes
    uint16_t flash_pgsize;      // flash page size in bytes
    uint16_t flash_bootsize;    // bootloader size (choose largest size) in bytes
    uint16_t flash_nrwwsize;    // No-Read-While-Write section size in bytes

    // available storage media
    uint32_t romsize;           // size of program memory in bytes
    uint32_t regsize;           // size of register file in bytes
    uint32_t ramsize;           // size of sram in bytes
    uint32_t eepsize;           // size of eeprom in bytse

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
    uint32_t ramstart;          // first address pointing to actual sram
    uint32_t romstart;          // start of mapped program memory (0 for unmapped)
    uint32_t eepstart;          // start of mapped eeprom (0 for unmapped)
    uint32_t memend;            // end of the data address space

    // memory-mapped register addresses
    uint16_t reg_rampz;
    uint16_t reg_eind;
    uint16_t reg_stack;
    uint16_t reg_status;
    uint16_t reg_spmcsr;
    uint16_t reg_mcucr;

    // important interrupt vectors
    uint32_t vec_spmrdy;        // store program memory completed

    // general purpose IO information
    uint16_t port_count;
    const struct avr_port *ports;

    // timers
    uint8_t timer_count;
    const struct avr_timer *timers;

    // register types and special behavior
    const struct avr_register *regmap;
};

extern const struct avr_model MODEL_ATMEGA1280;
extern const struct avr_model MODEL_ATMEGA2560;

#endif
