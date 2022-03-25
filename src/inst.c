#include <stdio.h>
#include "inst.h"

static int is_32bit_inst(uint16_t inst) {
    return (inst & 0xfc0f) == 0x9000 || // lds, sts
           (inst & 0xfe06) == 0x9406;   // jmp, call
}

static void set_sreg_add(struct avr *avr, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t status = avr->mem[avr->model.status_reg] & 0xc0;
    status |= (((a & b) | (~c & (a | b))) & 0x80) >> 7;     // carry
    status |= (c == 0) << 1;                                // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & b & ~c) | (~a & ~b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((a & b) | (~c & (a | b))) & 0x08) << 2;     // half carry
    avr->mem[avr->model.status_reg] = status;
}

static void set_sreg_sub(struct avr *avr, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t status = avr->mem[avr->model.status_reg] & 0xc0;
    status |= ((~a & b) | (b & c) | (~a & c)) >> 7;         // carry/borrow
    status |= (c == 0x00) << 1;                             // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & ~b & ~c) | (~a & b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((~a & b) | (c & (~a | b))) & 0x08) << 2;    // half carry
    avr->mem[avr->model.status_reg] = status;
}

void inst_add(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0xf),
            a = avr->mem[dst],
            b = avr->mem[src],
            c = avr->mem[dst] + avr->mem[src];
    avr->mem[dst] = c;
    printf("add\tr%d, r%d\n", dst, src);
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
    printf("adc\tr%d, r%d\n", dst, src);
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
    printf("adiw\tr%d, %d\n", dst_l, imm);
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
    printf("sub\tr%d, r%d\n", dst, src);
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_subi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) | 0x10,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->mem[dst],
            diff = val - imm;
    avr->mem[dst] = diff;
    printf("subi\tr%d, %d\n", dst, imm);
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
    printf("sbc\tr%d, r%d\n", dst, src);
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_sbci(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) | 0x10,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->mem[dst],
            diff = val - imm - (avr->mem[avr->model.status_reg] & 0x01);
    avr->mem[dst] = diff;
    printf("sbci\tr%d, %d\n", dst, imm);
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
    printf("sbiw\tr%d, %d\n", dst_l, imm);
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
    (void) avr;
    (void) inst;
    printf("and\n");
    avr->pc += 2;
}

void inst_andi(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("andi\n");
    avr->pc += 2;
}

void inst_or(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("or\n");
    avr->pc += 2;
}

void inst_ori(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ori\n");
    avr->pc += 2;
}

void inst_eor(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            val = avr->mem[dst] ^ avr->mem[src];
    avr->mem[dst] = val;
    printf("eor\tr%d, r%d\n", dst, src);
    uint8_t status = avr->mem[avr->model.status_reg] & 0xe1;
    status |= (val == 0x00) << 1;                           // zero
    status |= (val & 0x80) >> 5;                            // negative
    status |= (val & 0x80) >> 5;                            // sign
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
}

void inst_com(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("com\n");
    avr->pc += 2;
}

void inst_neg(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->mem[dst],
            neg = (~val)+1;
    avr->mem[dst] = neg;
    printf("neg r%d\n", dst);
    set_sreg_sub(avr, 0, val, neg);
    avr->pc += 2;
}

// void inst_sbr(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("sbr\n");
//     avr->pc += 2;
// }

// void inst_cbr(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("cbr\n");
//     avr->pc += 2;
// }

void inst_inc(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("inc\n");
    avr->pc += 2;
}

void inst_in(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            addr = (((inst >> 4) & 0x30) | (inst & 0xf)) + avr->model.in_out_offset;
    printf("in\tr%d, 0x%03x\n", dst, addr - avr->model.in_out_offset);
    avr->mem[dst] = avr->mem[addr];
    avr->pc += 2;
}

void inst_dec(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("dec\n");
    avr->pc += 2;
}

void inst_tst(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("tst\n");
    avr->pc += 2;
}

void inst_ser(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ser\n");
    avr->pc += 2;
}

void inst_mul(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->mem[reg1],
            b = avr->mem[reg2];
    uint16_t c = a * b;
    printf("mul\tr%d, r%d\n", reg1, reg2);
    avr->mem[0] = c & 0xff;
    avr->mem[1] = c >> 8;
    uint8_t status = avr->mem[avr->model.status_reg] & 0xfc;
    status |= (c & 0x8000) >> 15;                           // carry/borrow
    status |= (c == 0x00) << 1;                             // zero
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_muls(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("muls\n");
    avr->pc += 2;
}

void inst_mulsu(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("mulsu\n");
    avr->pc += 2;
}

void inst_fmul(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("fmul\n");
    avr->pc += 2;
}

void inst_fmuls(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("fmuls\n");
    avr->pc += 2;
}

void inst_fmulsu(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("fmulsu\n");
    avr->pc += 2;
}

void inst_rjmp(struct avr *avr, uint16_t inst) {
    int16_t dpc = (int16_t) (inst << 4) >> 4; // sign extend
    printf("rjmp\t%+d\n", 2*(dpc+1));
    avr->pc += 2*(dpc+1);
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_ijmp(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ijmp\n");
    avr->pc += 2;
}

void inst_eijmp(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("eijmp\n");
    avr->pc += 2;
}

void inst_jmp(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("jmp\n");
    avr->pc += 4;
}

void inst_rcall(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("rcall\n");
    avr->pc += 2;
}

void inst_icall(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("icall\n");
    avr->pc += 2;
}

void inst_eicall(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("eicall\n");
    avr->pc += 2;
}

void inst_call(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("call\n");
    avr->pc += 4;
}

void inst_ret(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ret\n");
    avr->pc += 2;
}

void inst_reti(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("reti\n");
    avr->pc += 2;
}

void inst_cpse(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("cpse\n");
    avr->pc += 2;
}

void inst_cp(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->mem[reg1],
            b = avr->mem[reg2],
            c = a - b;
    printf("cp \tr%d, r%d\n", reg1, reg2);
    uint8_t status = avr->mem[avr->model.status_reg] & 0xc0;
    status |= ((~a & b) | (b & c) | (~a & c)) >> 7;         // carry/borrow
    status |= (c == 0x00) << 1;                             // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & ~b & ~c) | (~a & b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((~a & b) | (c & (~a | b))) & 0x08) << 2;    // half carry
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
}

void inst_cpc(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->mem[reg1],
            b = avr->mem[reg2],
            c = a - b - (avr->mem[avr->model.status_reg] & 0x01);
    printf("cpc \tr%d, r%d\n", reg1, reg2);
    uint8_t status = avr->mem[avr->model.status_reg] & 0xc0;
    status |= ((~a & b) | (b & c) | (~a & c)) >> 7;         // carry/borrow
    status |= (c == 0x00) << 1;                             // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & ~b & ~c) | (~a & b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((~a & b) | (c & (~a | b))) & 0x08) << 2;    // half carry
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
}

void inst_cpi(struct avr *avr, uint16_t inst) {
    uint8_t reg = ((inst >> 4) & 0x0f) | 0x10,
            a = avr->mem[reg],
            b = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            c = a - b;
    printf("cpi\tr%d, %d\n", reg, b);
    uint8_t status = avr->mem[avr->model.status_reg] & 0xc0;
    status |= ((~a & b) | (b & c) | (~a & c)) >> 7;         // carry/borrow
    status |= (c == 0x00) << 1;                             // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & ~b & ~c) | (~a & b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((~a & b) | (c & (~a | b))) & 0x08) << 2;    // half carry
    avr->mem[avr->model.status_reg] = status;
    avr->pc += 2;
}

void inst_sbrc(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbrc\n");
    avr->pc += 2;
}

void inst_sbrs(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbrs\n");
    avr->pc += 2;
}

void inst_sbic(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbic\n");
    avr->pc += 2;
}

void inst_sbis(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbis\n");
    avr->pc += 2;
}

void inst_branch(struct avr *avr, uint16_t inst) {
    uint8_t dpc = ((int8_t) (inst >> 2)) >> 1,
            val = (inst >> 10) & 0x01,
            chk = (avr->mem[avr->model.status_reg] >> (inst & 0x07)) ^ val;
    printf("brch\t%+d on %cSREG[%d] (%d)", dpc, val ? '~' : ' ', inst & 0x07, avr->mem[avr->model.status_reg]);
    if (chk & 1) {
        avr->pc += 2*(dpc+1);
        avr->progress = 1;
        avr->status = CPU_STATUS_LONGINST;
        printf(" -> taken\n");
    } else {
        avr->pc += 2;
        printf(" -> pass\n");
    }
}
//
//
// void inst_brbs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brbs\n");
//     avr->pc += 2;
// }
//
// void inst_brbc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brbc\n");
//     avr->pc += 2;
// }
//
// void inst_breq(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("breq\n");
//     avr->pc += 2;
// }
//
// void inst_brne(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brne\n");
//     avr->pc += 2;
// }
//
// void inst_brcs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brcs\n");
//     avr->pc += 2;
// }
//
// void inst_brcc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brcc\n");
//     avr->pc += 2;
// }
//
// void inst_brsh(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brsh\n");
//     avr->pc += 2;
// }
//
// void inst_brlo(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brlo\n");
//     avr->pc += 2;
// }
//
// void inst_brmi(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brmi\n");
//     avr->pc += 2;
// }
//
// void inst_brpl(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brpl\n");
//     avr->pc += 2;
// }
//
// void inst_brge(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brge\n");
//     avr->pc += 2;
// }
//
// void inst_brlt(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brlt\n");
//     avr->pc += 2;
// }
//
// void inst_brhs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brhs\n");
//     avr->pc += 2;
// }
//
// void inst_brhc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brhc\n");
//     avr->pc += 2;
// }
//
// void inst_brts(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brts\n");
//     avr->pc += 2;
// }
//
// void inst_brtc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brtc\n");
//     avr->pc += 2;
// }
//
// void inst_brvs(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brvs\n");
//     avr->pc += 2;
// }
//
// void inst_brvc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brvc\n");
//     avr->pc += 2;
// }
//
// void inst_brie(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brie\n");
//     avr->pc += 2;
// }
//
// void inst_brid(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("brid\n");
//     avr->pc += 2;
// }

void inst_bit(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("bit\n");
    avr->pc += 2;
}

void inst_sbi(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbi\n");
    avr->pc += 2;
}

void inst_cbi(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("cbi\n");
    avr->pc += 2;
}

void inst_lsl(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("lsl\n");
    avr->pc += 2;
}

void inst_lsr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("lsr\n");
    avr->pc += 2;
}

void inst_rol(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("rol\n");
    avr->pc += 2;
}

void inst_ror(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ror\n");
    avr->pc += 2;
}

void inst_asr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("asr\n");
    avr->pc += 2;
}

void inst_swap(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("swap\n");
    avr->pc += 2;
}

void inst_bset(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("bset\n");
    avr->pc += 2;
}

void inst_bclr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("bclr\n");
    avr->pc += 2;
}

void inst_bst(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("bst\n");
    avr->pc += 2;
}

void inst_bld(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("bld\n");
    avr->pc += 2;
}
//
// void inst_sec(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("sec\n");
//     avr->pc += 2;
// }
//
// void inst_clc(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("clc\n");
//     avr->pc += 2;
// }
//
// void inst_sen(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("sen\n");
//     avr->pc += 2;
// }
//
// void inst_cln(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("cln\n");
//     avr->pc += 2;
// }
//
// void inst_sez(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("sez\n");
//     avr->pc += 2;
// }
//
// void inst_clz(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("clz\n");
//     avr->pc += 2;
// }
//
// void inst_sei(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("sei\n");
//     avr->pc += 2;
// }
//
// void inst_cli(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("cli\n");
//     avr->pc += 2;
// }
//
// void inst_ses(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("ses\n");
//     avr->pc += 2;
// }
//
// void inst_cls(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("cls\n");
//     avr->pc += 2;
// }
//
// void inst_sev(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("sev\n");
//     avr->pc += 2;
// }
//
// void inst_clv(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("clv\n");
//     avr->pc += 2;
// }
//
// void inst_set(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("set\n");
//     avr->pc += 2;
// }
//
// void inst_clt(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("clt\n");
//     avr->pc += 2;
// }
//
// void inst_seh(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("seh\n");
//     avr->pc += 2;
// }
//
// void inst_clh(struct avr *avr, uint16_t inst) {
//     (void) avr;
//     (void) inst;
//     printf("clh\n");
//     avr->pc += 2;
// }

void inst_mov(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("mov\n");
    avr->pc += 2;
}

void inst_movw(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("movw\n");
    avr->pc += 2;
}

void inst_ldi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) | 0x10,
            val = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    printf("ldi\tr%d, %d\n", dst, val);
    avr->mem[dst] = val;
    avr->pc += 2;
}

void inst_ld(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ld\n");
    avr->pc += 2;
}

void inst_ldd(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ldd\n");
    avr->pc += 2;
}

void inst_lds(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            addr_l = avr->rom[avr->pc+2],
            addr_h = avr->rom[avr->pc+3];
    uint16_t addr = (addr_h << 8) | addr_l;
    printf("lds\tr%d, 0x%04x\n", dst, addr);
    avr->mem[dst] = avr->mem[addr];
    avr->pc += 4;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_st(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("st\n");
    avr->pc += 2;
}

void inst_std(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("std\n");
    avr->pc += 2;
}

void inst_sts(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f,
            addr_l = avr->rom[avr->pc+2],
            addr_h = avr->rom[avr->pc+3];
    uint16_t addr = (addr_h << 8) | addr_l;
    printf("sts\t0x%04x, r%d\n", addr, src);
    avr->mem[addr] = avr->mem[src];
    avr->pc += 4;
    avr->progress = 1;
    avr->status = CPU_STATUS_LONGINST;
}

void inst_lpm(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("lpm\n");
    avr->pc += 2;
}

void inst_elpm(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("elpm\n");
    avr->pc += 2;
}

void inst_spm(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("spm\n");
    avr->pc += 2;
}

void inst_out(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("out\n");
    avr->pc += 2;
}

void inst_push(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("push\n");
    avr->pc += 2;
}

void inst_pop(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("pop\n");
    avr->pc += 2;
}

void inst_mcu(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("mcu\n");
    avr->pc += 2;
}

void inst_nop(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("nop\n");
    avr->pc += 2;
}

void inst_sleep(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sleep\n");
    avr->pc += 2;
}

void inst_wdr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("wdr\n");
    avr->pc += 2;
}

void inst_break(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("break\n");
    avr->pc += 2;
}
