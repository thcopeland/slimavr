#include <stdio.h>
#include <string.h>
#include "decode.h"

static void decode_add(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0xf);
    snprintf(str, size, "add\tr%d, r%d", dst, src);
}

static void decode_adc(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0xf);
    snprintf(str, size, "adc\tr%d, r%d", dst, src);
}

static void decode_adiw(char *str, int size, uint16_t inst) {
    uint8_t dst_l = ((inst >> 3) & 0x06) + 24,
            imm = ((inst >> 2) & 0x30) | (inst & 0x0f);
    snprintf(str, size, "adiw\tr%d, %d", dst_l, imm);
}

static void decode_sub(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "sub\tr%d, r%d", dst, src);
}

static void decode_subi(char *str, int size, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    snprintf(str, size, "subi\tr%d, %d", dst, imm);
}

static void decode_sbc(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "sbc\tr%d, r%d", dst, src);
}

static void decode_sbci(char *str, int size, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    snprintf(str, size, "sbci\tr%d, %d", dst, imm);
}

static void decode_sbiw(char *str, int size, uint16_t inst) {
    uint8_t dst_l = ((inst >> 3) & 0x06) + 24,
            imm = ((inst >> 2) & 0x30) | (inst & 0x0f);
    snprintf(str, size, "sbiw\tr%d, %d", dst_l, imm);
}

static void decode_and(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "and\tr%d, r%d", dst, src);
}

static void decode_andi(char *str, int size, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    snprintf(str, size, "andi\tr%d, %d", dst, imm);
}

static void decode_or(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "or\tr%d, r%d", dst, src);
}

static void decode_ori(char *str, int size, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    snprintf(str, size, "ori\tr%d, %d", dst, imm);
}

static void decode_eor(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "eor\tr%d, r%d", dst, src);
}

static void decode_com(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "com\tr%d", dst);
}

static void decode_neg(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "neg r%d", dst);
}

static void decode_inc(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "inc\tr%d", dst);
}

static void decode_in(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            addr = (((inst >> 5) & 0x30) | (inst & 0xf));
    snprintf(str, size, "in\tr%d, 0x%02x", dst, addr);
}

static void decode_dec(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "dec\tr%d", dst);
}

static void decode_mul(char *str, int size, uint16_t inst) {
    uint8_t r1 = (inst >> 4) & 0x1f,
            r2 = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "mul\tr%d, r%d", r1, r2);
}

static void decode_muls(char *str, int size, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x0f) + 16,
            r2 = (inst & 0x0f) + 16;
    snprintf(str, size, "muls\tr%d, r%d", r1, r2);
}

static void decode_mulsu(char *str, int size, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    snprintf(str, size, "mulsu\tr%d, r%d", r1, r2);
}

static void decode_fmul(char *str, int size, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    snprintf(str, size, "fmul\tr%d, r%d", r1, r2);
}

static void decode_fmuls(char *str, int size, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    snprintf(str, size, "fmuls\tr%d, r%d", r1, r2);
}

static void decode_fmulsu(char *str, int size, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    snprintf(str, size, "fmulsu\tr%d, r%d", r1, r2);
}

static void decode_rjmp(char *str, int size, uint16_t inst) {
    int16_t dpc = ((int16_t) (inst << 4) >> 3) + 2;
    snprintf(str, size, "rjmp\t%+d", dpc);
}

static void decode_ijmp(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "ijmp");
}

static void decode_eijmp(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "eijmp");
}

static void decode_jmp(char *str, int size, uint16_t inst, uint16_t inst2) {
    uint32_t addr = ((((inst & 0x01f0) >> 3) | (inst & 1)) << 17) | (inst2 << 1);
    snprintf(str, size, "jmp\t0x%06x", addr);
}

static void decode_rcall(char *str, int size, uint16_t inst) {
    int16_t diff = (int16_t) (inst << 4) >> 3;
    snprintf(str, size, "rcall\t%+d", diff);
}

static void decode_icall(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "icall");
}

static void decode_eicall(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "eicall");
}

static void decode_call(char *str, int size, uint16_t inst, uint16_t inst2) {
    uint32_t addr = ((((inst >> 3) & 0x3e) | (inst & 1)) << 16) | inst2;
    snprintf(str, size, "call\t0x%06x", 2*addr);
}

static void decode_ret(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "ret");
}

static void decode_reti(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "reti");
}

static void decode_cpse(char *str, int size, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "cpse\tr%d, r%d", reg1, reg2);
}

static void decode_cp(char *str, int size, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "cp\tr%d, r%d", reg1, reg2);
}

static void decode_cpc(char *str, int size, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "cpc\tr%d, r%d", reg1, reg2);
}

static void decode_cpi(char *str, int size, uint16_t inst) {
    uint8_t reg = ((inst >> 4) & 0x0f) + 16,
            b = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    snprintf(str, size, "cpi\tr%d, %d", reg, b);
}

static void decode_sbrc(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "sbrc\tr%d, %d", reg, inst & 0x7);
}

static void decode_sbrs(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "sbrs\tr%d, %d", reg, inst & 0x7);
}

static void decode_sbic(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 3) & 0x1f;
    snprintf(str, size, "sbic\t0x%02x, %d", reg, inst & 0x7);
}

static void decode_sbis(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 3) & 0x1f;
    snprintf(str, size, "sbis\t0x%02x, %d", reg, inst & 0x7);
}

static void decode_branch(char *str, int size, uint16_t inst) {
    int8_t diff = ((int8_t) (inst >> 2)) >> 1;
    uint8_t val = (inst >> 10) & 0x01;
    snprintf(str, size, "br%s\t%+d", (char*[]){"lo", "sh", "eq", "ne", "mi", "pl", "vs", "vc", "lt", "ge", "hs", "hc", "ts", "tc", "ie", "id"}[2*(inst & 0x07)+val], diff);
}

static void decode_sbi(char *str, int size, uint16_t inst) {
    uint8_t reg = ((inst >> 3) & 0x1f);
    snprintf(str, size, "sbi 0x%02x, %d", reg, inst & 0x7);
}

static void decode_cbi(char *str, int size, uint16_t inst) {
    uint8_t reg = ((inst >> 3) & 0x1f);
    snprintf(str, size, "cbi 0x%02x, %d", reg, inst & 0x7);
}

static void decode_lsr(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "lsr\tr%d", reg);
}

static void decode_ror(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "ror\tr%d", reg);
}

static void decode_asr(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "asr\tr%d", reg);
}

static void decode_swap(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "swap\tr%d", reg);
}

static void decode_bset(char *str, int size, uint16_t inst) {
    snprintf(str, size, "se%c", "cznvshti"[(inst>>4) & 0x7]);
}

static void decode_bclr(char *str, int size, uint16_t inst) {
    snprintf(str, size, "cl%c", "cznvshti"[(inst>>4) & 0x7]);
}

static void decode_bst(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "bst\tr%d, %d", reg, inst & 0x7);
}

static void decode_bld(char *str, int size, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f;
    snprintf(str, size, "bld\tr%d, %d", reg, inst & 0x7);
}

static void decode_mov(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f);
    snprintf(str, size, "mov\tr%d, r%d", dst, src);
}

static void decode_movw(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 3) & 0x1e,
            src = (inst << 1) & 0x1e;
    snprintf(str, size, "movw\tr%d, r%d", dst, src);
}

static void decode_ldi(char *str, int size, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            val = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    snprintf(str, size, "ldi\tr%d, %d", dst, val);
}

static void decode_ldx(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "ld\tr%d, %sX%s",
        dst,
        ((inst & 0x03) == 0x02) ? "-" : "",
        ((inst & 0x03) == 0x01) ? "+" : "");
}

static void decode_ldy(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "ld\tr%d, %sX%s",
        dst,
        ((inst & 0x03) == 0x02) ? "-" : "",
        ((inst & 0x03) == 0x01) ? "+" : "");
}

static void decode_ldz(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "ld\tr%d, %sX%s",
        dst,
        ((inst & 0x03) == 0x02) ? "-" : "",
        ((inst & 0x03) == 0x01) ? "+" : "");
}

static void decode_ldd(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            dsp = ((inst & 0x2000) >> 8) | ((inst & 0x0c00) >> 7) | (inst & 0x07);
    snprintf(str, size, "ldd\tr%d, %c+%d", dst, (inst & 0x08) ? 'Y' : 'Z', dsp);
}

static void decode_lds(char *str, int size, uint16_t inst, uint16_t inst2) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "lds\tr%d, 0x%04x", dst, inst2);
}

static void decode_stx(char *str, int size, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    snprintf(str, size, "st\t%sX%s, r%d",
        ((inst & 0x03) == 0x02) ? "-" : "",
        ((inst & 0x03) == 0x01) ? "+" : "",
        src);
}

static void decode_sty(char *str, int size, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    snprintf(str, size, "st\t%sY%s, r%d",
        ((inst & 0x03) == 0x02) ? "-" : "",
        ((inst & 0x03) == 0x01) ? "+" : "",
        src);
}

static void decode_stz(char *str, int size, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    snprintf(str, size, "st\t%sZ%s, r%d",
        ((inst & 0x03) == 0x02) ? "-" : "",
        ((inst & 0x03) == 0x01) ? "+" : "",
        src);
}

static void decode_std(char *str, int size, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f,
            dsp = ((inst & 0x2000) >> 8) | ((inst & 0x0c00) >> 7) | (inst & 0x07);
    snprintf(str, size, "std\t%c+%d, r%d", (inst & 0x08) ? 'Y' : 'Z', dsp, src);
}

static void decode_sts(char *str, int size, uint16_t inst, uint16_t inst2) {
    uint8_t src = (inst >> 4) & 0x1f;
    snprintf(str, size, "sts\t0x%04x, r%d", inst2, src);
}

static void decode_lpm(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "lpm\tr%d, Z%s", dst, inst & 1 ? "+" : "");
}

static void decode_elpm(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "elpm\tr%d, Z%s", dst, inst & 1 ? "+" : "");
}

static void decode_spm(char *str, int size, uint16_t inst) {
    snprintf(str, size, "spm%s", inst & 0x10 ? "\tZ+" : "");
}

static void decode_out(char *str, int size, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f,
            addr = (((inst >> 5) & 0x30) | (inst & 0xf));
    snprintf(str, size, "out\t0x%02x, r%d", addr, src);
}

static void decode_push(char *str, int size, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    snprintf(str, size, "push\tr%d", src);
}

static void decode_pop(char *str, int size, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    snprintf(str, size, "pop\tr%d", dst);
}

static void decode_nop(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "nop");
}

static void decode_sleep(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "sleep");
}

static void decode_wdr(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "wdr");
}

static void decode_break(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "break");
}

static void decode_xch(char *str, int size, uint16_t inst) {
    (void) inst;
    snprintf(str, size, "xch");
}

static void decode_lat(char *str, int size, uint16_t inst) {
    snprintf(str, size, "lat Z, r%d", (inst >> 4) & 0x1f);
}

static void decode_lac(char *str, int size, uint16_t inst) {
    snprintf(str, size, "lac Z, r%d", (inst >> 4) & 0x1f);
}

static void decode_las(char *str, int size, uint16_t inst) {
    snprintf(str, size, "las Z, r%d", (inst >> 4) & 0x1f);
}

static void decode_des(char *str, int size, uint16_t inst) {
    snprintf(str, size, "des %d", (inst >> 4) & 0x0f);
}

static void decode_invalid(char *str, int size, uint16_t inst) {
    snprintf(str, size, "unknown instruction 0x%04x", inst);
}

void avr_decode(char *str, int size, uint16_t inst, uint16_t inst2) {
    switch (inst & 0xfc00) {
        case 0x0000:
            if ((inst & 0xffff) == 0) decode_nop(str, size, inst);
            else if ((inst & 0xff00) == 0x0100) decode_movw(str, size, inst);
            else if ((inst & 0xff00) == 0x0200) decode_muls(str, size, inst);
            else if ((inst & 0xff88) == 0x0300) decode_mulsu(str, size, inst);
            else if ((inst & 0xff88) == 0x0308) decode_fmul(str, size, inst);
            else if ((inst & 0xff88) == 0x0380) decode_fmuls(str, size, inst);
            else if ((inst & 0xff88) == 0x0388) decode_fmulsu(str, size, inst);
            else decode_invalid(str, size, inst);
            break;
        case 0x0400:
            decode_cpc(str, size, inst);
            break;
        case 0x0800:
            decode_sbc(str, size, inst);
            break;
        case 0x0c00:
            decode_add(str, size, inst);
            break;
        case 0x1000:
            decode_cpse(str, size, inst);
            break;
        case 0x1400:
            decode_cp(str, size, inst);
            break;
        case 0x1800:
            decode_sub(str, size, inst);
            break;
        case 0x1c00:
            decode_adc(str, size, inst);
            break;
        case 0x2000:
            decode_and(str, size, inst);
            break;
        case 0x2400:
            decode_eor(str, size, inst);
            break;
        case 0x2800:
            decode_or(str, size, inst);
            break;
        case 0x2c00:
            decode_mov(str, size, inst);
            break;
        case 0x3000:
        case 0x3400:
        case 0x3800:
        case 0x3c00:
            decode_cpi(str, size, inst);
            break;
        case 0x4000:
        case 0x4400:
        case 0x4800:
        case 0x4c00:
            decode_sbci(str, size, inst);
            break;
        case 0x5000:
        case 0x5400:
        case 0x5800:
        case 0x5c00:
            decode_subi(str, size, inst);
            break;
        case 0x6000:
        case 0x6400:
        case 0x6800:
        case 0x6c00:
            decode_ori(str, size, inst);
            break;
        case 0x7000:
        case 0x7400:
        case 0x7800:
        case 0x7c00:
            decode_andi(str, size, inst);
            break;
        case 0x8000:
        case 0x8400:
        case 0x8800:
        case 0x8c00:
            if ((inst & 0x0200) == 0x0000) decode_ldd(str, size, inst);
            else decode_std(str, size, inst);
            break;
        case 0x9000:
            if ((inst & 0x0200) == 0x0000) {
                switch (inst & 0x000f) {
                    case 0x0000:
                        decode_lds(str, size, inst, inst2);
                        break;
                    case 0x0001:
                    case 0x0002:
                        decode_ldz(str, size, inst);
                        break;
                    case 0x0004:
                    case 0x0005:
                        decode_lpm(str, size, inst);
                        break;
                    case 0x0006:
                    case 0x0007:
                        decode_elpm(str, size, inst);
                        break;
                    case 0x0009:
                    case 0x000a:
                        decode_ldy(str, size, inst);
                        break;
                    case 0x000c:
                    case 0x000d:
                    case 0x000e:
                        decode_ldx(str, size, inst);
                        break;
                    case 0x000f:
                        decode_pop(str, size, inst);
                        break;
                    default:
                        decode_invalid(str, size, inst);
                        break;
                }
            } else {
                switch (inst & 0x000f) {
                    case 0x0000:
                        decode_sts(str, size, inst, inst2);
                        break;
                    case 0x0001:
                    case 0x0002:
                        decode_stz(str, size, inst);
                        break;
                    case 0x0004:
                        decode_xch(str, size, inst);
                        break;
                    case 0x0005:
                        decode_las(str, size, inst);
                        break;
                    case 0x0006:
                        decode_lac(str, size, inst);
                        break;
                    case 0x0007:
                        decode_lat(str, size, inst);
                        break;
                    case 0x0009:
                    case 0x000a:
                        decode_sty(str, size, inst);
                        break;
                    case 0x000c:
                    case 0x000d:
                    case 0x000e:
                        decode_stx(str, size, inst);
                        break;
                    case 0x000f:
                        decode_push(str, size, inst);
                        break;
                    default:
                        decode_invalid(str, size, inst);
                        break;
                }
            }
            break;
        case 0x9400:
            if ((inst & 0x020e) == 0x0008) {
                if ((inst & 0xff8f) == 0x9408) decode_bset(str, size, inst);
                else if ((inst & 0xff8f) == 0x9488) decode_bclr(str, size, inst);
                else if (inst == 0x9409) decode_ijmp(str, size, inst);
                else if (inst == 0x9419) decode_eijmp(str, size, inst);
                else if (inst == 0x9508) decode_ret(str, size, inst);
                else if (inst == 0x9509) decode_icall(str, size, inst);
                else if (inst == 0x9518) decode_reti(str, size, inst);
                else if (inst == 0x9519) decode_eicall(str, size, inst);
                else if (inst == 0x9588) decode_sleep(str, size, inst);
                else if (inst == 0x9598) decode_break(str, size, inst);
                else if (inst == 0x95a8) decode_wdr(str, size, inst);
                else if (inst == 0x95c8) decode_lpm(str, size, 0x9004);
                else if (inst == 0x95d8) decode_elpm(str, size, 0x9006);
                else if (inst == 0x95e8) decode_spm(str, size, inst);
                else if (inst == 0x95f8) decode_spm(str, size, inst);
                else decode_invalid(str, size, inst);
            } else if ((inst & 0xfe00) == 0x9400) {
                switch (inst & 0x000f) {
                    case 0x0000:
                        decode_com(str, size, inst);
                        break;
                    case 0x0001:
                        decode_neg(str, size, inst);
                        break;
                    case 0x0002:
                        decode_swap(str, size, inst);
                        break;
                    case 0x0003:
                        decode_inc(str, size, inst);
                        break;
                    case 0x0005:
                        decode_asr(str, size, inst);
                        break;
                    case 0x0006:
                        decode_lsr(str, size, inst);
                        break;
                    case 0x0007:
                        decode_ror(str, size, inst);
                        break;
                    case 0x000a:
                        decode_dec(str, size, inst);
                        break;
                    case 0x000b:
                        if ((inst & 0x0100) == 0x0000) decode_des(str, size, inst);
                        else decode_invalid(str, size, inst);
                        break;
                    case 0x000c:
                    case 0x000d:
                        decode_jmp(str, size, inst, inst2);
                        break;
                    case 0x000e:
                    case 0x000f:
                        decode_call(str, size, inst, inst2);
                        break;
                    default:
                        decode_invalid(str, size, inst);
                        break;
                }
            } else if ((inst & 0xff00) == 0x9600) decode_adiw(str, size, inst);
            else if ((inst & 0xff00) == 0x9700) decode_sbiw(str, size, inst);
            else decode_invalid(str, size, inst);
            break;
        case 0x9800:
            if ((inst & 0x0300) == 0x0000) decode_cbi(str, size, inst);
            else if ((inst & 0x0300) == 0x0100) decode_sbic(str, size, inst);
            else if ((inst & 0x0300) == 0x0200) decode_sbi(str, size, inst);
            else decode_sbis(str, size, inst);
            break;
        case 0x9c00:
            decode_mul(str, size, inst);
            break;
        case 0xa000:
        case 0xa400:
        case 0xa800:
        case 0xac00:
            if ((inst & 0x0200) == 0x0000) decode_ldd(str, size, inst);
            else decode_std(str, size, inst);
            break;
        case 0xb000:
        case 0xb400:
            decode_in(str, size, inst);
            break;
        case 0xb800:
        case 0xbc00:
            decode_out(str, size, inst);
            break;
        case 0xc000:
        case 0xc400:
        case 0xc800:
        case 0xcc00:
            decode_rjmp(str, size, inst);
            break;
        case 0xd000:
        case 0xd400:
        case 0xd800:
        case 0xdc00:
            decode_rcall(str, size, inst);
            break;
        case 0xe000:
        case 0xe400:
        case 0xe800:
        case 0xec00:
            decode_ldi(str, size, inst);
            break;
        case 0xf000:
        case 0xf400:
            decode_branch(str, size, inst);
            break;
        case 0xf800:
            if ((inst & 0x0200) == 0x0000) decode_bld(str, size, inst);
            else decode_bst(str, size, inst);
            break;
        case 0xfc00:
            if ((inst & 0x0008) == 0) {
                if ((inst & 0x0200) == 0x0000) decode_sbrc(str, size, inst);
                else decode_sbrs(str, size, inst);
            } else {
                decode_invalid(str, size, inst);
            }
            break;
        default:
            decode_invalid(str, size, inst);
            break;
    }
}
