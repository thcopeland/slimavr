#include <stdio.h>
#include "inst.h"
#include "opt.h"

static int is_32bit_inst(uint16_t inst) {
    return (inst & 0xfc0f) == 0x9000 || // lds, sts
           (inst & 0xfe06) == 0x9406;   // jmp, call
}

static inline void set_sreg_add(struct avr *avr, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t status = avr->mem[avr->model.status_reg] & 0xc0;
    status |= (((a & b) | (~c & (a | b))) & 0x80) >> 7;     // carry
    status |= (c == 0) << 1;                                // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & b & ~c) | (~a & ~b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((a & b) | (~c & (a | b))) & 0x08) << 2;     // half carry
    avr->mem[avr->model.status_reg] = status;
}

static inline void set_sreg_sub(struct avr *avr, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t status = avr->mem[avr->model.status_reg] & 0xc0;
    status |= ((~a & b) | (b & c) | (~a & c)) >> 7;         // carry/borrow
    status |= (c == 0x00) << 1;                             // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & ~b & ~c) | (~a & b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((~a & b) | (c & (~a | b))) & 0x08) << 2;    // half carry
    avr->mem[avr->model.status_reg] = status;
}

static inline void set_sreg_logical(struct avr *avr, uint8_t val) {
    uint8_t status = avr->mem[avr->model.status_reg] & 0xe1;
    status |= (val == 0) << 1;                              // zero
    status |= (val & 0x80) >> 5;                            // negative
    status |= (val & 0x80) >> 3;                            // sign
    avr->mem[avr->model.status_reg] = status;
}

static inline void set_sreg_mul(struct avr *avr, uint16_t val) {
    uint8_t status = avr->mem[avr->model.status_reg] & 0xfc;
    status |= (val & 0x8000) >> 15;                         // carry
    status |= (val == 0x00) << 1;                           // zero
    avr->mem[avr->model.status_reg] = status;
}

static inline void set_sreg_fmul(struct avr *avr, uint16_t val) {
    uint8_t status = avr->mem[avr->model.status_reg] & 0xfc;
    status |= (val & 0x8000) >> 15;                         // carry
    status |= (val == 0) << 1;                              // zero
    avr->mem[avr->model.status_reg] = status;
}

void inst_add(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0xf),
            a = avr->mem[dst],
            b = avr->mem[src],
            c = avr->mem[dst] + avr->mem[src];
    avr->mem[dst] = c;
    LOG("add\tr%d, r%d\n", dst, src);
    set_sreg_add(avr, a, b, c);
    avr->pc += 2;
}

void inst_adc(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0xf),
            a = avr->mem[dst],
            b = avr->mem[src],
            c = a + b + (avr->mem[avr->model.status_reg] & 0x01);
    avr->mem[dst] = c;
    LOG("adc\tr%d, r%d\n", dst, src);
    set_sreg_add(avr, a, b, c);
    avr->pc += 2;
}

void inst_adiw(struct avr *avr, uint16_t inst) {
    uint8_t dst_l = ((inst >> 3) & 0x06) + 24,
            dst_h = dst_l + 1,
            imm = ((inst >> 2) & 0x30) | (inst & 0x0f),
            val_l = avr->mem[dst_l],
            val_h = avr->mem[dst_h],
            status = avr->mem[avr->model.status_reg] & 0xe0;
    uint16_t sum = ((uint16_t) val_h << 8) + (uint16_t) val_l + imm;
    LOG("adiw\tr%d, %d\n", dst_l, imm);
    avr->mem[dst_l] = sum & 0xff;
    avr->mem[dst_h] = sum >> 8;
    status |= (~(sum >> 8) & val_h) >> 7;                   // carry
    status |= (sum == 0) << 1;                              // zero
    status |= (sum >> 13) & 0x40;                           // negative
    status |= (((sum >> 8) & ~val_h) & 0x80) >> 4;          // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_sub(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->mem[dst],
            b = avr->mem[src],
            c = a - b;
    avr->mem[dst] = c;
    LOG("sub\tr%d, r%d\n", dst, src);
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_subi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->mem[dst],
            diff = val - imm;
    avr->mem[dst] = diff;
    LOG("subi\tr%d, %d\n", dst, imm);
    set_sreg_sub(avr, val, imm, diff);
    avr->pc += 2;
}

void inst_sbc(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->mem[dst],
            b = avr->mem[src],
            c = a - b - (avr->mem[avr->model.status_reg] & 0x01);
    avr->mem[dst] = c;
    LOG("sbc\tr%d, r%d\n", dst, src);
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_sbci(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->mem[dst],
            diff = val - imm - (avr->mem[avr->model.status_reg] & 0x01);
    avr->mem[dst] = diff;
    LOG("sbci\tr%d, %d\n", dst, imm);
    set_sreg_sub(avr, val, imm, diff);
    avr->pc += 2;
}

void inst_sbiw(struct avr *avr, uint16_t inst) {
    uint8_t dst_l = ((inst >> 3) & 0x06) + 24,
            dst_h = dst_l + 1,
            imm = ((inst >> 2) & 0x30) | (inst & 0x0f),
            val_l = avr->mem[dst_l],
            val_h = avr->mem[dst_h],
            status = avr->mem[avr->model.status_reg] & 0xe0;
    uint16_t diff = ((uint16_t) val_h << 8) + (uint16_t) val_l - imm;
    LOG("sbiw\tr%d, %d\n", dst_l, imm);
    avr->mem[dst_l] = diff & 0xff;
    avr->mem[dst_h] = diff >> 8;
    status |= ((diff >> 8) & ~val_h) >> 7;                  // carry
    status |= (diff == 0) << 1;                             // zero
    status |= (diff >> 13) & 0x40;                          // negative
    status |= ((~(diff >> 8) & val_h) & 0x80) >> 4;         // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_and(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            val = avr->mem[dst] & avr->mem[src];
    avr->mem[dst] = val;
    LOG("and\tr%d, r%d\n", dst, src);
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_andi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->mem[dst] & imm;
    avr->mem[dst] = val;
    LOG("andi\tr%d, %d\n", dst, imm);
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_or(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            val = avr->mem[dst] | avr->mem[src];
    avr->mem[dst] = val;
    LOG("or\tr%d, r%d\n", dst, src);
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_ori(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->mem[dst] | imm;
    avr->mem[dst] = val;
    LOG("ori\tr%d, %d\n", dst, imm);
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_eor(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            val = avr->mem[dst] ^ avr->mem[src];
    avr->mem[dst] = val;
    LOG("eor\tr%d, r%d\n", dst, src);
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_com(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->mem[dst];
    avr->mem[dst] = ~val;
    LOG("com r%d\n", dst);
    set_sreg_sub(avr, 255, val, ~val);
}

void inst_neg(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->mem[dst];
    avr->mem[dst] = -val;
    LOG("neg r%d\n", dst);
    set_sreg_sub(avr, 0, val, -val);
    avr->pc += 2;
}

void inst_inc(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->mem[dst],
            inc = val+1,
            status = avr->mem[avr->model.status_reg] & 0xe1;
    avr->mem[dst] = inc;
    LOG("inc\tr%d\n", dst);
    status |= (inc == 0) << 1;                              // zero
    status |= (inc & 0x80) >> 5;                            // negative
    status |= ((inc ^ 0x7f) == 0xff) << 3;                  // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
}

void inst_in(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            addr = (((inst >> 4) & 0x30) | (inst & 0xf)) + avr->model.in_out_offset;
    LOG("in\tr%d, 0x%03x\n", dst, addr - avr->model.in_out_offset);
    avr->mem[dst] = avr->mem[addr];
    avr->pc += 2;
}

void inst_dec(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->mem[dst],
            dec = val-1,
            status = avr->mem[avr->model.status_reg] & 0xe1;
    avr->mem[dst] = dec;
    LOG("dec\tr%d\n", dst);
    status |= (dec == 0) << 1;                              // zero
    status |= (dec & 0x80) >> 5;                            // negative
    status |= ((dec ^ 0x80) == 0xff) << 3;                  // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
}

void inst_mul(struct avr *avr, uint16_t inst) {
    uint8_t r1 = (inst >> 4) & 0x1f,
            r2 = ((inst >> 5) & 0x10) | (inst & 0x0f);
    uint16_t prod = (uint16_t) avr->mem[r1] * avr->mem[r2];
    avr->mem[0] = prod & 0xff;
    avr->mem[1] = prod >> 8;
    LOG("mul\tr%d, r%d\n", r1, r2);
    set_sreg_mul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_muls(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x0f) + 16,
            r2 = (inst & 0x0f) + 16;
    uint16_t prod = (int8_t) avr->mem[r1] * (int8_t) avr->mem[r2];
    avr->mem[0] = prod & 0xff;
    avr->mem[1] = (prod >> 8) & 0xff;
    LOG("muls\tr%d, r%d\n", r1, r2);
    set_sreg_mul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_mulsu(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = (int8_t) avr->mem[r1] * (uint8_t) avr->mem[r2];
    avr->mem[0] = prod & 0xff;
    avr->mem[1] = (prod >> 8) & 0xff;
    LOG("mulsu\tr%d, r%d\n", r1, r2);
    set_sreg_mul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_fmul(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = avr->mem[r1] * avr->mem[r2];
    LOG("fmul\tr%d, r%d\n", r1, r2);
    avr->mem[0] = (prod << 1) & 0xff;
    avr->mem[1] = (prod >> 7) & 0xff;
    set_sreg_fmul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_fmuls(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = (int8_t) avr->mem[r1] * (int8_t) avr->mem[r2];
    LOG("fmuls\tr%d, r%d\n", r1, r2);
    avr->mem[0] = (prod << 1) & 0xff;
    avr->mem[1] = (prod >> 7) & 0xff;
    set_sreg_fmul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_fmulsu(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = (int8_t) avr->mem[r1] * (uint8_t) avr->mem[r2];
    LOG("fmulsu\tr%d, r%d\n", r1, r2);
    avr->mem[0] = (prod << 1) & 0xff;
    avr->mem[1] = (prod >> 7) & 0xff;
    set_sreg_fmul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_rjmp(struct avr *avr, uint16_t inst) {
    int16_t dpc = (int16_t) (inst << 4) >> 4; // sign extend
    LOG("rjmp\t%+d\n", 2*(dpc+1));
    avr->pc += 2*(dpc+1);
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_ijmp(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("ijmp\n");
    avr->pc += 2;
}

void inst_eijmp(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("eijmp\n");
    avr->pc += 2;
}

void inst_jmp(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("jmp\n");
    avr->pc += 4;
}

void inst_rcall(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("rcall\n");
    avr->pc += 2;
}

void inst_icall(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("icall\n");
    avr->pc += 2;
}

void inst_eicall(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("eicall\n");
    avr->pc += 2;
}

void inst_call(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("call\n");
    avr->pc += 4;
}

void inst_ret(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("ret\n");
    avr->pc += 2;
}

void inst_reti(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("reti\n");
    avr->pc += 2;
}

void inst_cpse(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("cpse\n");
    avr->pc += 2;
}

void inst_cp(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->mem[reg1],
            b = avr->mem[reg2],
            c = a - b;
    LOG("cp\tr%d, r%d\n", reg1, reg2);
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_cpc(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->mem[reg1],
            b = avr->mem[reg2],
            c = a - b - (avr->mem[avr->model.status_reg] & 0x01);
    LOG("cpc\tr%d, r%d\n", reg1, reg2);
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_cpi(struct avr *avr, uint16_t inst) {
    uint8_t reg = ((inst >> 4) & 0x0f) + 16,
            a = avr->mem[reg],
            b = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            c = a - b;
    LOG("cpi\tr%d, %d\n", reg, b);
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_sbrc(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("sbrc\n");
    avr->pc += 2;
}

void inst_sbrs(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("sbrs\n");
    avr->pc += 2;
}

void inst_sbic(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("sbic\n");
    avr->pc += 2;
}

void inst_sbis(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("sbis\n");
    avr->pc += 2;
}

void inst_branch(struct avr *avr, uint16_t inst) {
    uint8_t dpc = ((int8_t) (inst >> 2)) >> 1,
            val = (inst >> 10) & 0x01,
            chk = (avr->mem[avr->model.status_reg] >> (inst & 0x07)) ^ val;
    LOG("brch\t%+d on %cSREG[%d] (%d)", dpc, val ? '~' : ' ', inst & 0x07, avr->mem[avr->model.status_reg]);
    if (chk & 1) {
        avr->pc += 2*(dpc+1);
        avr->progress = 1;
        avr->status = CPU_STATUS_LONGINST;
        LOG(" -> taken\n");
    } else {
        avr->pc += 2;
        LOG(" -> pass\n");
    }
}
//
//
// void inst_brbs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brbs\n");
//     avr->pc += 2;
// }
//
// void inst_brbc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brbc\n");
//     avr->pc += 2;
// }
//
// void inst_breq(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("breq\n");
//     avr->pc += 2;
// }
//
// void inst_brne(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brne\n");
//     avr->pc += 2;
// }
//
// void inst_brcs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brcs\n");
//     avr->pc += 2;
// }
//
// void inst_brcc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brcc\n");
//     avr->pc += 2;
// }
//
// void inst_brsh(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brsh\n");
//     avr->pc += 2;
// }
//
// void inst_brlo(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brlo\n");
//     avr->pc += 2;
// }
//
// void inst_brmi(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brmi\n");
//     avr->pc += 2;
// }
//
// void inst_brpl(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brpl\n");
//     avr->pc += 2;
// }
//
// void inst_brge(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brge\n");
//     avr->pc += 2;
// }
//
// void inst_brlt(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brlt\n");
//     avr->pc += 2;
// }
//
// void inst_brhs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brhs\n");
//     avr->pc += 2;
// }
//
// void inst_brhc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brhc\n");
//     avr->pc += 2;
// }
//
// void inst_brts(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brts\n");
//     avr->pc += 2;
// }
//
// void inst_brtc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brtc\n");
//     avr->pc += 2;
// }
//
// void inst_brvs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brvs\n");
//     avr->pc += 2;
// }
//
// void inst_brvc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brvc\n");
//     avr->pc += 2;
// }
//
// void inst_brie(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brie\n");
//     avr->pc += 2;
// }
//
// void inst_brid(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("brid\n");
//     avr->pc += 2;
// }

void inst_bit(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("bit\n");
    avr->pc += 2;
}

void inst_sbi(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("sbi\n");
    avr->pc += 2;
}

void inst_cbi(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("cbi\n");
    avr->pc += 2;
}

void inst_lsl(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("lsl\n");
    avr->pc += 2;
}

void inst_lsr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("lsr\n");
    avr->pc += 2;
}

void inst_rol(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("rol\n");
    avr->pc += 2;
}

void inst_ror(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("ror\n");
    avr->pc += 2;
}

void inst_asr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("asr\n");
    avr->pc += 2;
}

void inst_swap(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("swap\n");
    avr->pc += 2;
}

void inst_bset(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("bset\n");
    avr->pc += 2;
}

void inst_bclr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("bclr\n");
    avr->pc += 2;
}

void inst_bst(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("bst\n");
    avr->pc += 2;
}

void inst_bld(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("bld\n");
    avr->pc += 2;
}
//
// void inst_sec(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("sec\n");
//     avr->pc += 2;
// }
//
// void inst_clc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("clc\n");
//     avr->pc += 2;
// }
//
// void inst_sen(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("sen\n");
//     avr->pc += 2;
// }
//
// void inst_cln(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("cln\n");
//     avr->pc += 2;
// }
//
// void inst_sez(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("sez\n");
//     avr->pc += 2;
// }
//
// void inst_clz(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("clz\n");
//     avr->pc += 2;
// }
//
// void inst_sei(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("sei\n");
//     avr->pc += 2;
// }
//
// void inst_cli(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("cli\n");
//     avr->pc += 2;
// }
//
// void inst_ses(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("ses\n");
//     avr->pc += 2;
// }
//
// void inst_cls(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("cls\n");
//     avr->pc += 2;
// }
//
// void inst_sev(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("sev\n");
//     avr->pc += 2;
// }
//
// void inst_clv(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("clv\n");
//     avr->pc += 2;
// }
//
// void inst_set(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("set\n");
//     avr->pc += 2;
// }
//
// void inst_clt(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("clt\n");
//     avr->pc += 2;
// }
//
// void inst_seh(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("seh\n");
//     avr->pc += 2;
// }
//
// void inst_clh(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     LOG("clh\n");
//     avr->pc += 2;
// }

void inst_mov(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("mov\n");
    avr->pc += 2;
}

void inst_movw(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("movw\n");
    avr->pc += 2;
}

void inst_ldi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            val = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    LOG("ldi\tr%d, %d\n", dst, val);
    avr->mem[dst] = val;
    avr->pc += 2;
}

void inst_ld(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("ld\n");
    avr->pc += 2;
}

void inst_ldd(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("ldd\n");
    avr->pc += 2;
}

void inst_lds(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            addr_l = avr->rom[avr->pc+2],
            addr_h = avr->rom[avr->pc+3];
    uint16_t addr = (addr_h << 8) | addr_l;
    LOG("lds\tr%d, 0x%04x\n", dst, addr);
    avr->mem[dst] = avr->mem[addr];
    avr->pc += 4;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_st(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("st\n");
    avr->pc += 2;
}

void inst_std(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("std\n");
    avr->pc += 2;
}

void inst_sts(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f,
            addr_l = avr->rom[avr->pc+2],
            addr_h = avr->rom[avr->pc+3];
    uint16_t addr = (addr_h << 8) | addr_l;
    LOG("sts\t0x%04x, r%d\n", addr, src);
    avr->mem[addr] = avr->mem[src];
    avr->pc += 4;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_lpm(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("lpm\n");
    avr->pc += 2;
}

void inst_elpm(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("elpm\n");
    avr->pc += 2;
}

void inst_spm(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("spm\n");
    avr->pc += 2;
}

void inst_out(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("out\n");
    avr->pc += 2;
}

void inst_push(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("push\n");
    avr->pc += 2;
}

void inst_pop(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("pop\n");
    avr->pc += 2;
}

void inst_mcu(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("mcu\n");
    avr->pc += 2;
}

void inst_nop(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("nop\n");
    avr->pc += 2;
}

void inst_sleep(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("sleep\n");
    avr->pc += 2;
}

void inst_wdr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("wdr\n");
    avr->pc += 2;
}

void inst_break(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    LOG("break\n");
    avr->pc += 2;
}
