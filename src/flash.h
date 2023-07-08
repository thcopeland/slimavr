#ifndef SLIMAVR_FLASH_H
#define SLIMAVR_FLASH_H

#include <stdint.h>
#include <stddef.h>

#define AVR_SPMCSR_SPMIE  0x80
#define AVR_SPMCSR_RWWSB  0x40
#define AVR_SPMCSR_SIGRD  0x20
#define AVR_SPMCSR_RWWSRE 0x10
#define AVR_SPMCSR_BLBSET 0x08
#define AVR_SPMCSR_PGWRT  0x04
#define AVR_SPMCSR_PGERS  0x02
#define AVR_SPMCSR_SPMEN  0x01

struct avr_flash_state {
  uint8_t *buffer;    // internal page buffer
  uint32_t addr;      // flash write address
  uint16_t progress;  // progress of flash write/erase (0 - complete)
  uint8_t operation;  // whether writing or erasing a flash page
  uint8_t spm_mode;   // how to interpret a subsequent SPM instruction
  uint8_t status;     // [interrupt:1][unused:3][access window timer:4]
  uint8_t idle;       // if set, no flash checks or updates are performed
};

struct avr;

void *avr_flash_allocate_internal(struct avr *avr);
void avr_flash_free_internal(struct avr *avr);
void avr_flash_reset(struct avr *avr);
void avr_set_flash_reg(struct avr *avr, uint16_t addr, uint8_t val, uint8_t mask);
void avr_exec_spm(struct avr *avr, uint16_t inst);
void avr_update_flash(struct avr *avr);

#endif
