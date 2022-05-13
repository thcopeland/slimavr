#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>
#include "avr.h"

#define AVR_SPM_SPIME 0x80
#define AVR_SPM_RWWSB 0x40
#define AVR_SPM_SIGRD 0x20
#define AVR_SPM_RWWSRE 0x10
#define AVR_SPM_BLBSET 0x08
#define AVR_SPM_PGWRT 0x04
#define AVR_SPM_PGERS 0x02
#define AVR_SPM_SPMEN 0x01

#define AVR_SPM_READY 0x80
#define AVR_SPM_CLOCK 0x07
#define AVR_SPM_TIMEOUT 4

void flash_erase_page(struct avr *avr, uint32_t addr);
void flash_write_page(struct avr *avr, uint32_t addr);
void flash_write_buff(struct avr *avr, uint32_t addr, uint8_t low, uint8_t high);
void flash_set_blb(struct avr *avr, uint8_t val);

#endif
