#include <string.h>
#include <stdlib.h>
#include "avr.h"
#include "opt.h"
#include "avrdefs.h"
#include "flash.h"

#include <stdio.h>

#define AVR_STATUS_INTERRUPT 0x80
#define AVR_STATUS_TIMER     0x0f

#define AVR_SPM_OP_NONE  0
#define AVR_SPM_OP_WRITE 1
#define AVR_SPM_OP_ERASE 2

#define AVR_SPM_MODE_NONE   0
#define AVR_SPM_MODE_BUFFER 1
#define AVR_SPM_MODE_RWWSRE 2
#define AVR_SPM_MODE_BLBSET 3
#define AVR_SPM_MODE_PGWRT  4
#define AVR_SPM_MODE_PGERS  5
#define AVR_SPM_MODE_SIGRD  6

void avr_init_flash_state(struct avr_flash_state *flash, size_t buffsize) {
    flash->buffer = malloc(buffsize);
    if (flash->buffer != NULL) {
        memset(flash->buffer, 0xff, buffsize);
    }
    flash->progress = 0;
    flash->addr = 0;
    flash->operation = AVR_SPM_OP_NONE;
    flash->spm_mode = AVR_SPM_MODE_NONE;
    flash->status = 0;
}

void avr_free_flash_state(struct avr_flash_state *flash) {
    free(flash->buffer);
}

void avr_set_flash_reg(struct avr *avr, uint16_t addr, uint8_t val, uint8_t mask) {
    struct avr_flash_state *flash = &avr->flash_data;
    uint8_t ctrl = (avr->mem[addr] & ~mask) | (val & mask);
    avr->mem[addr] = ctrl;

    if (val & mask & AVR_SPMCSR_SPMEN) {
        flash->status = 4;

        if ((val & mask & 0x3f) == (AVR_SPMCSR_SPMEN | AVR_SPMCSR_RWWSRE)) {
            flash->spm_mode = AVR_SPM_MODE_RWWSRE;
        } else if ((val & mask & 0x3f) == (AVR_SPMCSR_SPMEN | AVR_SPMCSR_BLBSET)) {
            // unsupported
        } else if ((val & mask & 0x3f) == (AVR_SPMCSR_SPMEN | AVR_SPMCSR_PGWRT)) {
            flash->spm_mode = AVR_SPM_MODE_PGWRT;
        } else if ((val & mask & 0x3f) == (AVR_SPMCSR_SPMEN | AVR_SPMCSR_PGERS)) {
            flash->spm_mode = AVR_SPM_MODE_PGERS;
        } else if ((val & mask & 0x3f) == (AVR_SPMCSR_SPMEN | AVR_SPMCSR_SIGRD)) {
            // unsupported
        } else if ((val & mask & 0x3f) == AVR_SPMCSR_SPMEN) {
            flash->spm_mode = AVR_SPM_MODE_BUFFER;
        } else {
            flash->spm_mode = AVR_SPM_MODE_NONE;
        }
    }
}

static void avr_flash_complete(struct avr *avr) {
    struct avr_flash_state *flash = &avr->flash_data;

    if (flash->operation == AVR_SPM_OP_ERASE) {
        if (flash->addr + avr->model.flash_pgsize >= avr->model.romsize) {
            LOG("*** Cannot erase flash page starting at 0x%x *** \n", flash->addr);
            avr->status = CPU_STATUS_CRASHED;
            avr->error = CPU_INVALID_ROM_ADDRESS;
        } else {
            memset(avr->rom+flash->addr, 0xff, avr->model.flash_pgsize);
            avr->reg[avr->model.reg_spmcsr] &= ~(AVR_SPMCSR_PGERS | AVR_SPMCSR_SPMEN);

            if (avr->mem[avr->model.reg_spmcsr] & AVR_SPMCSR_SPMIE) {
                flash->status |= AVR_STATUS_INTERRUPT;
            }
        }
    } else if (flash->operation == AVR_SPM_OP_WRITE) {
        if (flash->addr + avr->model.flash_pgsize >= avr->model.romsize) {
            LOG("*** Cannot write flash page starting at 0x%x *** \n", flash->addr);
            avr->status = CPU_STATUS_CRASHED;
            avr->error = CPU_INVALID_ROM_ADDRESS;
        } else {
            memcpy(avr->rom+flash->addr, flash->buffer, avr->model.flash_pgsize);
            memset(flash->buffer, 0xff, avr->model.flash_pgsize);
            avr->reg[avr->model.reg_spmcsr] &= ~(AVR_SPMCSR_PGWRT | AVR_SPMCSR_SPMEN);

            if (avr->mem[avr->model.reg_spmcsr] & AVR_SPMCSR_SPMIE) {
                flash->status |= AVR_STATUS_INTERRUPT;
            }
        }
    }

    flash->progress = 0;
    flash->operation = AVR_SPM_OP_NONE;
}

void avr_exec_spm(struct avr *avr, uint16_t inst) {
    struct avr_flash_state *flash = &avr->flash_data;

    if (avr->reg[avr->model.reg_spmcsr] & AVR_SPMCSR_SPMEN) {
        uint32_t pg_mask = avr->model.flash_pgsize - 1;
        pg_mask |= pg_mask >> 16;
        pg_mask |= pg_mask >> 8;
        pg_mask |= pg_mask >> 4;
        pg_mask |= pg_mask >> 2;
        pg_mask |= pg_mask >> 1;

        uint32_t addr = ((uint32_t) avr->reg[AVR_REG_Z+1] << 8) | avr->reg[AVR_REG_Z];
        if (avr->model.romsize > 0xffff) {
            addr |= (uint32_t) avr->reg[avr->model.reg_rampz] << 16;
        }

        if (flash->spm_mode == AVR_SPM_MODE_RWWSRE) {
            // re-enable the RWW section
            avr->mem[avr->model.reg_spmcsr] &= ~(AVR_SPMCSR_SPMEN|AVR_SPMCSR_RWWSB);
        } else if (flash->spm_mode == AVR_SPM_MODE_PGERS) {
            flash->addr = addr & ~pg_mask;
            flash->progress = 64000;
            flash->operation = AVR_SPM_OP_ERASE;
            if (addr > avr->model.romsize - avr->model.flash_nrwwsize) {
                // finish erase immediately (in reality, the CPU is halted until completion)
                avr_flash_complete(avr);
            } else {
                avr->mem[avr->model.reg_spmcsr] |= AVR_SPMCSR_RWWSB;
            }
        } else if (flash->spm_mode == AVR_SPM_MODE_PGWRT) {
            flash->addr = addr & ~pg_mask;
            flash->progress = 64000;
            flash->operation = AVR_SPM_OP_WRITE;
            if (addr > avr->model.romsize - avr->model.flash_nrwwsize) {
                // finish write immediately
                avr_flash_complete(avr);
            } else {
                avr->mem[avr->model.reg_spmcsr] |= AVR_SPMCSR_RWWSB;
            }
        } else if (flash->spm_mode == AVR_SPM_MODE_BUFFER) {
            uint32_t pg_addr = addr & 0xfffffffe & pg_mask;
            if (flash->buffer[pg_addr] == 0xff)   flash->buffer[pg_addr] = avr->reg[AVR_REG_R0];
            if (flash->buffer[pg_addr+1] == 0xff) flash->buffer[pg_addr+1] = avr->reg[AVR_REG_R1];

            if (inst & 0x10) { // post-increment
                addr += 2;
                if (avr->model.romsize > 0xffff) {
                    avr->reg[avr->model.reg_rampz] = addr >> 16;
                }
                avr->reg[AVR_REG_Z+1] = addr >> 8;
                avr->reg[AVR_REG_Z] = addr & 0xff;
            }
        }
    }
}

void avr_update_flash(struct avr *avr) {
    struct avr_flash_state *flash = &avr->flash_data;

    if (flash->progress > 0) {
        flash->progress--;
        if (flash->progress == 0) {
            avr_flash_complete(avr);
        }
    }

    if ((flash->status & AVR_STATUS_TIMER) > 0) {
        flash->status--;
        if (flash->status == 0) {
            flash->spm_mode = AVR_SPM_MODE_NONE;

            if (flash->operation == AVR_SPM_OP_NONE) {
                avr->reg[avr->model.reg_spmcsr] &= ~(AVR_SPMCSR_SPMEN | AVR_SPMCSR_PGERS | AVR_SPMCSR_PGWRT);
            }
        }
    }
}
