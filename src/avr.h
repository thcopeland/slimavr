#ifndef AVR_H
#define AVR_H

#include <stdint.h>
#include "model.h"
#include "eeprom.h"
#include "flash.h"

enum avr_error {
    CPU_INVALID_INSTRUCTION,
    CPU_UNSUPPORTED_INSTRUCTION,
    CPU_INVALID_RAM_ADDRESS,
    CPU_INVALID_ROM_ADDRESS,
    CPU_INVALID_STACK_ACCESS
};

enum avr_status {
    CPU_STATUS_NORMAL,
    CPU_STATUS_CRASHED,
    CPU_STATUS_COMPLETING,
    CPU_STATUS_INTERRUPTING,
    CPU_STATUS_IDLE
};

enum avr_pending_type {
    AVR_PENDING_NONE,
    AVR_PENDING_COPY
};

struct avr_pending_inst {
    uint32_t src;
    uint32_t dst;
    enum avr_pending_type type;
};

struct avr {
    struct avr_model model;     // processor model
    enum avr_error error;       // current error, if any
    enum avr_status status;     // processor state
    int8_t progress;            // cycles remaining for multi-cycle instructions
    uint32_t pc;                // program counter
    uint64_t clock;             // number of cycles
    uint64_t insts;             // number of instructions

    // memory
    uint8_t *mem;

    // memory segments
    uint8_t *rom;               // program memory
    uint8_t *reg;               // registers
    uint8_t *ram;               // sram
    uint8_t *eep;               // eeprom

    // various internal state
    struct avr_timerstate *timer_data;
    struct avr_pending_inst pending_inst;
    struct avr_eeprom_state eeprom_data;
    struct avr_flash_state flash_data;
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
 * Read an IO register's value. This can be used for basic communication, but
 * if you need more control, you should access avr->reg or avr->mem directly.
 */
uint8_t avr_io_read(struct avr *avr, uint16_t reg);


/*
 * Write to an IO register. This can be used for basic communication, but
 * if you need more control, you should access avr->reg or avr->mem directly.
 */
void avr_io_write(struct avr *avr, uint16_t reg, uint8_t val);

#endif
