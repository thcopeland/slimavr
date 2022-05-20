#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "model.h"
#include "inst.h"
#include "interrupt.h"
#include "opt.h"
#include "avr.h"

static void alloc_avr_memory(struct avr *avr) {
    // In order to simplify and speed up data accesses, all data segments (register
    // file, SRAM, EEPROM, program memory) are actually just pointers into a
    // single contiguous block of memory, structured to match the address space.
    //
    // However, the program memory and EEPROM often aren't mapped into the data
    // address space so we account for them separately if necessary.
    uint32_t rom_offset = avr->model.romstart,
             eep_offset = avr->model.eepstart,
             unmapped = 0;

    if (avr->model.romstart == 0) {
        rom_offset = avr->model.memend + unmapped;
        unmapped += avr->model.romsize;
    }

    if (avr->model.eepstart == 0) {
        eep_offset = avr->model.memend + unmapped;
        unmapped += avr->model.eepsize;
    }

    avr->mem = malloc(avr->model.memend + unmapped);
    if (avr->mem == NULL) {
        return;
    } else {
        // set up each segment
        avr->reg = avr->mem;
        avr->ram = avr->mem + avr->model.ramstart;
        avr->rom = avr->mem + rom_offset;
        avr->eep = avr->mem + eep_offset;
        memset(avr->reg, 0, avr->model.regsize);
    }

    avr->flash_pgbuff = malloc(avr->model.flash_pgsize);
    if (avr->flash_pgbuff == NULL) {
        free(avr->mem);
        avr->mem = NULL;
        return;
    }

    avr->timer_data = malloc(sizeof(avr->timer_data[0])*avr->model.timer_count);
    if (avr->timer_data == NULL) {
        free(avr->mem);
        free(avr->flash_pgbuff);
        avr->mem = NULL;
        return;
    } else {
        for (int i = 0; i < avr->model.timer_count; i++) {
            timerstate_init(avr->timer_data+i);
        }
    }
}

struct avr *avr_init(struct avr_model model) {
    check_compatibility();
    struct avr *avr = malloc(sizeof(*avr));
    if (avr) {
        avr->error = 0;
        avr->status = CPU_STATUS_NORMAL;
        avr->progress = 0;
        avr->model = model;
        avr->pc = 0;
        avr->clock = 0;
        avr->insts = 0;

        alloc_avr_memory(avr);

        if (avr->mem == NULL) {
            free(avr);
            avr = NULL;
        } else {
            uint16_t sp = model.ramstart+model.ramsize - 1;
            avr->reg[model.reg_stack+1] = sp >> 8;
            avr->reg[model.reg_stack] = sp & 0xff;
        }
    }
    return avr;
}

void avr_free(struct avr *avr) {
    if (avr) {
        free(avr->mem);
        free(avr->flash_pgbuff);
        free(avr->timer_data);
        free(avr);
    }
}

static inline void avr_update(struct avr *avr) {
    avr->clock++;

    avr_update_timers(avr);

    avr_check_interrupts(avr);
}

static inline void avr_exec(struct avr *avr) {
    uint8_t inst_l = avr->rom[avr->pc],
            inst_h = avr->rom[avr->pc+1];
    uint16_t inst = (inst_h << 8) + inst_l;
    avr->insts++;

    LOG("%x:\t", avr->pc);

    // instructions of the form 0000-00xx-xxxx-xxxx
    // nop, movw, muls, mulsu, fmul, fmuls, fmulsu
    if ((inst_h & 0xfc) == 0x00) {
        if (inst == 0x00) inst_nop(avr, inst);
        else if (inst_h == 0x01) inst_movw(avr, inst);
        else if (inst_h == 0x02) inst_muls(avr, inst);
        else if (inst_h == 0x03) {
            if ((inst_l & 0x88) == 0x00) inst_mulsu(avr, inst);
            else if ((inst_l & 0x88) == 0x08) inst_fmul(avr, inst);
            else if ((inst_l & 0x88) == 0x80) inst_fmuls(avr, inst);
            else if ((inst_l & 0x88) == 0x88) inst_fmulsu(avr, inst);
            else {
                avr->error = CPU_INVALID_INSTRUCTION;
                avr->status = CPU_STATUS_CRASHED;
            }
        } else {
            avr->error = CPU_INVALID_INSTRUCTION;
            avr->status = CPU_STATUS_CRASHED;
        }
    }
    // instructions of the form 000x-xxxx-xxxx-xxxx
    // cp, cpc, sub, sbc, add, adc, cpse, and, eor, or, mov, cpi
    else if ((inst_h & 0xc0) == 0x00) {
        if ((inst_h & 0xe0) == 0x00) {
            if ((inst_h & 0xfc) == 0x04) inst_cpc(avr, inst);
            else if ((inst_h & 0xfc) == 0x14) inst_cp(avr, inst);
            else if ((inst_h & 0xfc) == 0x08) inst_sbc(avr, inst);
            else if ((inst_h & 0xfc) == 0x18) inst_sub(avr, inst);
            else if ((inst_h & 0xfc) == 0x1c) inst_adc(avr, inst);
            else if ((inst_h & 0xfc) == 0x0c) inst_add(avr, inst);
            else if ((inst_h & 0xfc) == 0x10) inst_cpse(avr, inst);
            else {
                avr->error = CPU_INVALID_INSTRUCTION;
                avr->status = CPU_STATUS_CRASHED;
            }
        } else {
            if ((inst_h & 0xfc) == 0x20) inst_and(avr, inst);
            else if ((inst_h & 0xfc) == 0x24) inst_eor(avr, inst);
            else if ((inst_h & 0xfc) == 0x28) inst_or(avr, inst);
            else if ((inst_h & 0xfc) == 0x2c) inst_mov(avr, inst);
            else inst_cpi(avr, inst);
        }
    }
    // instructions of the form 01xx-xxxx-xxxx-xxxx
    // sbci, subi, ori, andi
    else if ((inst_h & 0xc0) == 0x40) {
        if ((inst_h & 0xf0) == 0x40) inst_sbci(avr, inst);
        else if ((inst_h & 0xf0) == 0x50) inst_subi(avr, inst);
        else if ((inst_h & 0xf0) == 0x60) inst_ori(avr, inst);
        else inst_andi(avr, inst);
    }
    // instructions of the form 10x0-xxxx-xxxx-xxxx
    // ldd, std
    else if ((inst_h & 0xd0) == 0x80) {
        if (inst_h & 0x02) inst_std(avr, inst);
        else inst_ldd(avr, inst);
    }
    // instructions of the form 1001-00xx-xxxx-xxxx
    // lds, sts, ld, st, lpm, elpm, xch, las, lac, lat, push, pop
    else if ((inst_h & 0xfc) == 0x90) {
        if (inst_h & 0x02) {
            if ((inst_l & 0x0f) == 0x00) inst_sts(avr, inst);
            else if ((inst_l & 0x0c) == 0x08) inst_sty(avr, inst);
            else if ((inst_l & 0x0c) == 0x00) inst_stz(avr, inst);
            else if ((inst_l & 0x0f) == 0x0f) inst_push(avr, inst);
            else if ((inst_l & 0x0f) >= 0xc) inst_stx(avr, inst);
            else {
                avr->error = CPU_INVALID_INSTRUCTION;
                avr->status = CPU_STATUS_CRASHED;
            }
        } else {
            if ((inst_l & 0x0f) == 0x00) inst_lds(avr, inst);
            else if ((inst_l & 0x0c) == 0x08) inst_ldy(avr, inst);
            else if ((inst_l & 0x0c) == 0x00) inst_ldz(avr, inst);
            else if ((inst_l & 0x0f) == 0x0f) inst_pop(avr, inst);
            else if ((inst_l & 0x0f) >= 0x0c) inst_ldx(avr, inst);
            else if ((inst_l & 0x0e) == 0x04) inst_lpm(avr, inst);
            else if ((inst_l & 0x0e) == 0x06) inst_elpm(avr, inst);
            else { // xch, las, lac, lat
                avr->error = CPU_INVALID_INSTRUCTION;
                avr->status = CPU_STATUS_CRASHED;
            }
        }
    }
    // instructions of the form 1001-010x-xxxx-xxxx
    else if ((inst_h & 0xfe) == 0x94) {
        // instructions of the form 1001-010x-xxxx-0xxx
        // com, neg, swap, inc, asr, lsr, ror
        if ((inst_l & 0x08) == 0x00) {
            if ((inst_l & 0x0f) == 0x00) inst_com(avr, inst);
            else if ((inst_l & 0x0f) == 0x01) inst_neg(avr, inst);
            else if ((inst_l & 0x0f) == 0x02) inst_swap(avr, inst);
            else if ((inst_l & 0x0f) == 0x03) inst_inc(avr, inst);
            else if ((inst_l & 0x0f) == 0x05) inst_asr(avr, inst);
            else if ((inst_l & 0x0f) == 0x06) inst_lsr(avr, inst);
            else if ((inst_l & 0x0f) == 0x07) inst_ror(avr, inst);
            else {
                avr->error = CPU_INVALID_INSTRUCTION;
                avr->status = CPU_STATUS_CRASHED;
            }
        }
        // instructions of the form 1001-0101-xxxx-1000
        // ret, reti, sleep, break, wdr, lpm (r0), elpm (r0), spm
        else if ((inst_h & 0xff) == 0x95 && (inst_l & 0xf) == 0x08) {
            if (inst_l == 0x08) inst_ret(avr, inst);
            else if (inst_l == 0x18) inst_reti(avr, inst);
            else if (inst_l == 0x88) inst_sleep(avr, inst);
            else if (inst_l == 0xc8) inst_lpm(avr, 0x9004); // forward to general form
            else if (inst_l == 0xd8) inst_elpm(avr, 0x9006); // forward to general form
            else if ((inst_l & 0xef) == 0xe8) inst_spm(avr, inst);
            else { // including break, wdr
                avr->error = CPU_INVALID_INSTRUCTION;
                avr->status = CPU_STATUS_CRASHED;
            }
        }
        // instructions of the form 1001-010x-xxxx-xxxx
        // se*, cl*, eijmp, icall, dec, des, jmp, call
        else if ((inst_h & 0x01) == 0x00 && (inst_l & 0x0f) == 0x08) {
            if (inst_l & 0x80) inst_bclr(avr, inst);
            else inst_bset(avr, inst);
        } else if (inst_l == 0x09) {
            if (inst_h & 0x01) inst_icall(avr, inst);
            else inst_ijmp(avr, inst);
        } else if (inst_l == 0x19) {
            if (inst_h & 0x01) inst_eicall(avr, inst);
            else inst_eijmp(avr, inst);
        } else if ((inst_l & 0x0f) == 0x0a) inst_dec(avr, inst);
        else if ((inst_l & 0x0e) == 0x0c) inst_jmp(avr, inst);
        else if ((inst_l & 0x0e) == 0x0e) inst_call(avr, inst);
        else {
            avr->error = CPU_INVALID_INSTRUCTION;
            avr->status = CPU_STATUS_CRASHED;
        }
    }
    // instructions of the form 1001-xxxx-xxxx-xxxx
    // adiw, sbiw, cbi, sbi, sbic, sbis, mul
    else if ((inst_h & 0xf0) == 0x90) {
        if (inst_h == 0x96) inst_adiw(avr, inst);
        else if (inst_h == 0x97) inst_sbiw(avr, inst);
        else if (inst_h == 0x9a) inst_sbi(avr, inst);
        else if (inst_h == 0x98) inst_cbi(avr, inst);
        else if (inst_h == 0x9b) inst_sbis(avr, inst);
        else if (inst_h == 0x99) inst_sbic(avr, inst);
        else if ((inst_h & 0xfc) == 0x9c) inst_mul(avr, inst);
    }
    // in, out, rjmp, rcall, ldi, br**, bld, bst, sbrs, sbrc
    else if ((inst_h & 0xf8) == 0xb0) inst_in(avr, inst);
    else if ((inst_h & 0xf8) == 0xb8) inst_out(avr, inst);
    else if ((inst_h & 0xf0) == 0xc0) inst_rjmp(avr, inst);
    else if ((inst_h & 0xf0) == 0xd0) inst_rcall(avr, inst);
    else if ((inst_h & 0xf0) == 0xe0) inst_ldi(avr, inst);
    else if ((inst_h & 0xf8) == 0xf0) inst_branch(avr, inst);
    else if ((inst_h & 0xfe) == 0xf8) inst_bld(avr, inst);
    else if ((inst_h & 0xfe) == 0xfa) inst_bst(avr, inst);
    else if ((inst_h & 0xfe) == 0xfe) inst_sbrs(avr, inst);
    else if ((inst_h & 0xfe) == 0xfc) inst_sbrc(avr, inst);
    else {
        avr->error = CPU_INVALID_INSTRUCTION;
        avr->status = CPU_STATUS_CRASHED;
    }
}

void avr_step(struct avr *avr) {
    if (avr->status == CPU_STATUS_CRASHED) {
        return;
    }

    avr_update(avr);

    if (avr->status == CPU_STATUS_COMPLETING) {
        LOG("*** continuing last instruction (%d) ***\n", avr->progress);
        avr->progress--;
        if (avr->progress <= 0) {
            avr->status = CPU_STATUS_NORMAL;
        }
        return;
    }

    if (avr->status == CPU_STATUS_INTERRUPTING) {
        LOG("*** interrupting (%d) ***\n", avr->progress);
        avr->progress--;
        if (avr->progress <= 0) {
            avr->status = CPU_STATUS_NORMAL;
        }
        return;
    }

    if (avr->status == CPU_STATUS_NORMAL) {
        avr_exec(avr);
    }
}
