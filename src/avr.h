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
 * Read a register's value. For most registers, this is equivalent to reading
 * avr->reg directly, but a fair number have specialized behavior, including
 * changing CPU state. In most cases, you won't want to trigger these side
 * effects and should access avr->reg or avr->mem directly.
 *
 * Note: This is idempotent and used in the IN and LD* instruction implementations.
 *
 * Note: This interface may be changed or removed at some point.
 */
uint8_t avr_get_reg(struct avr *avr, uint16_t reg);

/*
 * Set a register's value. This will trigger the proper interrupts and any other
 * side effects. For some registers (eg IO), you should use this function, but
 * in general, it will be preferable to write avr->reg directly.
 *
 * This is faster than but equivalent to avr_set_reg_bits(avr, reg, val, 0xff).
 *
 * Note: This is idempotent and used in the OUT and ST* instructions implementations.
 *
 * Note: This interface may be changed or removed at some point.
 */
void avr_set_reg(struct avr *avr, uint16_t reg, uint8_t val);

/*
 * Set a register's value, but only the bits specifed in the given mask are affected.
 * This is only useful for a few registers, such as EECR. This will trigger the
 * proper interrupts and any other side effects. In general, it will be preferable to
 * use avr_set_reg (which avr_set_reg_bits delegates most calls to anyway) or access
 * avr->reg directly.
 *
 * Note: This is idempotent and used in the SBI and CBI instructions implementations.
 *
 * Note: This interface may be changed or removed at some point.
 */
void avr_set_reg_bits(struct avr *avr, uint16_t reg, uint8_t val, uint8_t mask);

#endif
