#include "dispatch.h"

void avr_dispatch(struct avr *avr, uint8_t inst_l, uint8_t inst_h) {
    uint16_t inst = (inst_h << 8) + inst_l;

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
