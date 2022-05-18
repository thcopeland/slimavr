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
    struct avr_model model;     // processor model
    uint8_t error;              // current error, if any
    uint8_t status;             // processor state
    int8_t progress;            // cycles remaining for multi-cycle instructions
    uint32_t pc;                // program counter
    uint64_t clock;             // number of cycles
    uint64_t insts;             // number of instructions
    uint8_t tmps[16];           // internal temporary registers

    // memory
    uint8_t *mem;

    // memory segments
    uint8_t *rom;               // program memory
    uint8_t *reg;               // registers
    uint8_t *ram;               // sram
    uint8_t *eep;               // eeprom

    struct avr_timerstate *timer_data;  // internal timer state

    uint8_t *flash_pgbuff;      // program memory buffer
};

/*
 * Allocate and initialize a new avr instance of the given model.
 */
struct avr *avr_init(struct avr_model model);

/*
 * Free an avr instance.
 */
void avr_free(struct avr *avr);

/*
 * Step a single cycle.
 */
void avr_step(struct avr *avr);

/*
 * Read a register's value. [[This is equivalent to reading directly from avr->reg
 * or avr->mem, included for read/write symmetry. TODO: for now, anyway]]
 */
uint8_t avr_get_reg(struct avr *avr, uint16_t reg);

/*
 * Set a register's value. This will trigger the proper interrupts and any other
 * side effects. If you don't want/care about these, it may be preferable to
 * write avr->reg directly.
 */
int avr_set_reg(struct avr *avr, uint16_t reg, uint8_t val);

#endif
