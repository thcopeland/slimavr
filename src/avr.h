#ifndef SLIMAVR_AVR_H
#define SLIMAVR_AVR_H

#include <stdint.h>
#include "model.h"
#include "eeprom.h"
#include "flash.h"
#include "gpio.h"

enum avr_error {
    AVR_INVALID_INSTRUCTION,
    AVR_UNSUPPORTED_INSTRUCTION,
    AVR_INVALID_RAM_ADDRESS,
    AVR_INVALID_ROM_ADDRESS,
    AVR_INVALID_STACK_ACCESS
};

enum avr_status {
    MCU_STATUS_NORMAL,
    MCU_STATUS_CRASHED,
    MCU_STATUS_COMPLETING,
    MCU_STATUS_INTERRUPTING,
    MCU_STATUS_IDLE
};

enum avr_incomplete_inst_type {
    AVR_INCOMPLETE_NONE,
    AVR_INCOMPLETE_COPY
};

struct avr_incomplete_inst {
    uint32_t src;
    uint32_t dst;
    enum avr_incomplete_inst_type type;
};

#define avr_panic(avr, err) ({                                                  \
    avr->error = err;                                                           \
    avr->status = MCU_STATUS_CRASHED;                                           \
})

struct avr_tracedata;

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
    enum avr_pin_state (*pin_data)[8];
    struct avr_timerstate *timer_data;
    struct avr_incomplete_inst incomplete_inst;
    struct avr_eeprom_state eeprom_data;
    struct avr_flash_state flash_data;
    struct avr_tracedata *trace;
};

/*
 * Allocate and initialize a new avr instance of the given model.
 */
struct avr *avr_new(struct avr_model model);

/*
 * Reset an avr instance, resetting registers, buffers, and memory to their
 * initial state.
 */
void avr_reset(struct avr *avr);

/*
 * Free an avr instance.
 */
void avr_free(struct avr *avr);

/*
 * Dump the contents of the register file and recent instructions. If fname is
 * null, writes to stdout.
 */
int avr_dump(struct avr *avr, const char *fname);

/*
 * Step a single cycle.
 */
void avr_step(struct avr *avr);

#endif
