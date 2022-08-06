#include <string.h>
#include "flash.h"

void flash_erase_page(struct avr *avr, uint32_t addr) {
    uint32_t mask = avr->model.flash_pgsize - 1;
    mask |= mask >> 16;
    mask |= mask >> 8;
    mask |= mask >> 4;
    mask |= mask >> 2;
    mask |= mask >> 1;
    addr &= ~mask;
    if (addr >= avr->model.romsize) {
        avr->status = CPU_STATUS_CRASHED;
        avr->error = CPU_INVALID_ROM_ADDRESS;
    } else {
        memset(avr->rom+addr, 0xff, avr->model.flash_pgsize);
        // Mark as immediately complete. This is inaccurate for any writes outside
        // the NRWW section, but simple and compatible.
        avr->reg[avr->model.reg_spmcsr] &= ~(AVR_SPM_PGERS|AVR_SPM_SPMEN);
    }
}

void flash_write_page(struct avr *avr, uint32_t addr) {
    uint32_t mask = avr->model.flash_pgsize - 1;
    mask |= mask >> 16;
    mask |= mask >> 8;
    mask |= mask >> 4;
    mask |= mask >> 2;
    mask |= mask >> 1;
    addr &= ~mask;
    if (addr >= avr->model.romsize) {
        avr->status = CPU_STATUS_CRASHED;
        avr->error = CPU_INVALID_ROM_ADDRESS;
    } else {
        memcpy(avr->rom+addr, avr->flash_pgbuff, avr->model.flash_pgsize);
        // Mark as immediately complete. This is inaccurate for any writes outside
        // the NRWW section, but simple and compatible.
        avr->spm_status |= 1;
        avr->reg[avr->model.reg_spmcsr] &= ~(AVR_SPM_PGWRT|AVR_SPM_SPMEN);
    }
}

void flash_write_buff(struct avr *avr, uint32_t addr, uint8_t low, uint8_t high) {
    uint32_t mask = avr->model.flash_pgsize - 1;
    mask |= mask >> 16;
    mask |= mask >> 8;
    mask |= mask >> 4;
    mask |= mask >> 2;
    mask |= mask >> 1;
    addr &= mask & 0xfffffffe;
    avr->flash_pgbuff[addr] = low;
    avr->flash_pgbuff[addr+1] = high;
    // Mark as immediately complete. This is inaccurate for any writes outside
    // the NRWW section, but simple and compatible.
    avr->reg[avr->model.reg_spmcsr] &= ~AVR_SPM_SPMEN;
}

void flash_set_blb(struct avr *avr, uint8_t val) {
    (void) val;
    // TODO support setting bootloader bits (requires changing (e)lpm )
    avr->status = CPU_STATUS_CRASHED;
    avr->error = CPU_UNSUPPORTED_INSTRUCTION;
}
