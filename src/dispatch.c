#include "dispatch.h"

void avr_dispatch(struct avr *avr, uint16_t inst) {
    switch (inst & 0xfc00) {
        case 0x0000:
            if ((inst & 0xffff) == 0) inst_nop(avr, inst);
            else if ((inst & 0xff00) == 0x0100) inst_movw(avr, inst);
            else if ((inst & 0xff00) == 0x0200) inst_muls(avr, inst);
            else if ((inst & 0xff88) == 0x0300) inst_mulsu(avr, inst);
            else if ((inst & 0xff88) == 0x0308) inst_fmul(avr, inst);
            else if ((inst & 0xff88) == 0x0380) inst_fmuls(avr, inst);
            else if ((inst & 0xff88) == 0x0388) inst_fmulsu(avr, inst);
            else goto invalid_instruction;
            break;
        case 0x0400:
            inst_cpc(avr, inst);
            break;
        case 0x0800:
            inst_sbc(avr, inst);
            break;
        case 0x0c00:
            inst_add(avr, inst);
            break;
        case 0x1000:
            inst_cpse(avr, inst);
            break;
        case 0x1400:
            inst_cp(avr, inst);
            break;
        case 0x1800:
            inst_sub(avr, inst);
            break;
        case 0x1c00:
            inst_adc(avr, inst);
            break;
        case 0x2000:
            inst_and(avr, inst);
            break;
        case 0x2400:
            inst_eor(avr, inst);
            break;
        case 0x2800:
            inst_or(avr, inst);
            break;
        case 0x2c00:
            inst_mov(avr, inst);
            break;
        case 0x3000:
        case 0x3400:
        case 0x3800:
        case 0x3c00:
            inst_cpi(avr, inst);
            break;
        case 0x4000:
        case 0x4400:
        case 0x4800:
        case 0x4c00:
            inst_sbci(avr, inst);
            break;
        case 0x5000:
        case 0x5400:
        case 0x5800:
        case 0x5c00:
            inst_subi(avr, inst);
            break;
        case 0x6000:
        case 0x6400:
        case 0x6800:
        case 0x6c00:
            inst_ori(avr, inst);
            break;
        case 0x7000:
        case 0x7400:
        case 0x7800:
        case 0x7c00:
            inst_andi(avr, inst);
            break;
        case 0x8000:
        case 0x8400:
        case 0x8800:
        case 0x8c00:
            if ((inst & 0x0200) == 0x0000) inst_ldd(avr, inst);
            else inst_std(avr, inst);
            break;
        case 0x9000:
            if ((inst & 0x0200) == 0x0000) {
                switch (inst & 0x000f) {
                    case 0x0000:
                        inst_lds(avr, inst);
                        break;
                    case 0x0001:
                    case 0x0002:
                        inst_ldz(avr, inst);
                        break;
                    case 0x0004:
                    case 0x0005:
                        inst_lpm(avr, inst);
                        break;
                    case 0x0006:
                    case 0x0007:
                        inst_elpm(avr, inst);
                        break;
                    case 0x0009:
                    case 0x000a:
                        inst_ldy(avr, inst);
                        break;
                    case 0x000c:
                    case 0x000d:
                    case 0x000e:
                        inst_ldx(avr, inst);
                        break;
                    case 0x000f:
                        inst_pop(avr, inst);
                        break;
                    default:
                        goto invalid_instruction;
                        break;
                }
            } else {
                switch (inst & 0x000f) {
                    case 0x0000:
                        inst_sts(avr, inst);
                        break;
                    case 0x0001:
                    case 0x0002:
                        inst_stz(avr, inst);
                        break;
                    case 0x0004:
                        inst_xch(avr, inst);
                        break;
                    case 0x0005:
                        inst_las(avr, inst);
                        break;
                    case 0x0006:
                        inst_lac(avr, inst);
                        break;
                    case 0x0007:
                        inst_lat(avr, inst);
                        break;
                    case 0x0009:
                    case 0x000a:
                        inst_sty(avr, inst);
                        break;
                    case 0x000c:
                    case 0x000d:
                    case 0x000e:
                        inst_stx(avr, inst);
                        break;
                    case 0x000f:
                        inst_push(avr, inst);
                        break;
                    default:
                        goto invalid_instruction;
                        break;
                }
            }
            break;
        case 0x9400:
            if ((inst & 0x020e) == 0x0008) {
                if ((inst & 0xff8f) == 0x9408) inst_bset(avr, inst);
                else if ((inst & 0xff8f) == 0x9488) inst_bclr(avr, inst);
                else if (inst == 0x9409) inst_ijmp(avr, inst);
                else if (inst == 0x9419) inst_eijmp(avr, inst);
                else if (inst == 0x9508) inst_ret(avr, inst);
                else if (inst == 0x9509) inst_icall(avr, inst);
                else if (inst == 0x9518) inst_reti(avr, inst);
                else if (inst == 0x9519) inst_eicall(avr, inst);
                else if (inst == 0x9588) inst_sleep(avr, inst);
                else if (inst == 0x9598) inst_break(avr, inst);
                else if (inst == 0x95a8) inst_wdr(avr, inst);
                else if (inst == 0x95c8) inst_lpm(avr, 0x9004);
                else if (inst == 0x95d8) inst_elpm(avr, 0x9006);
                else if (inst == 0x95e8) inst_spm(avr, inst);
                else if (inst == 0x95f8) inst_spm(avr, inst);
                else goto invalid_instruction;
            } else if ((inst & 0xfe00) == 0x9400) {
                switch (inst & 0x000f) {
                    case 0x0000:
                        inst_com(avr, inst);
                        break;
                    case 0x0001:
                        inst_neg(avr, inst);
                        break;
                    case 0x0002:
                        inst_swap(avr, inst);
                        break;
                    case 0x0003:
                        inst_inc(avr, inst);
                        break;
                    case 0x0005:
                        inst_asr(avr, inst);
                        break;
                    case 0x0006:
                        inst_lsr(avr, inst);
                        break;
                    case 0x0007:
                        inst_ror(avr, inst);
                        break;
                    case 0x000a:
                        inst_dec(avr, inst);
                        break;
                    case 0x000b:
                        if ((inst & 0x0100) == 0x0000) inst_des(avr, inst);
                        else goto invalid_instruction;
                        break;
                    case 0x000c:
                    case 0x000d:
                        inst_jmp(avr, inst);
                        break;
                    case 0x000e:
                    case 0x000f:
                        inst_call(avr, inst);
                        break;
                    default:
                        goto invalid_instruction;
                        break;
                }
            } else if ((inst & 0xff00) == 0x9600) inst_adiw(avr, inst);
            else if ((inst & 0xff00) == 0x9700) inst_sbiw(avr, inst);
            else goto invalid_instruction;
            break;
        case 0x9800:
            if ((inst & 0x0300) == 0x0000) inst_cbi(avr, inst);
            else if ((inst & 0x0300) == 0x0100) inst_sbic(avr, inst);
            else if ((inst & 0x0300) == 0x0200) inst_sbi(avr, inst);
            else inst_sbis(avr, inst);
            break;
        case 0x9c00:
            inst_mul(avr, inst);
            break;
        case 0xa000:
        case 0xa400:
        case 0xa800:
        case 0xac00:
            if ((inst & 0x0200) == 0x0000) inst_ldd(avr, inst);
            else inst_std(avr, inst);
            break;
        case 0xb000:
        case 0xb400:
            inst_in(avr, inst);
            break;
        case 0xb800:
        case 0xbc00:
            inst_out(avr, inst);
            break;
        case 0xc000:
        case 0xc400:
        case 0xc800:
        case 0xcc00:
            inst_rjmp(avr, inst);
            break;
        case 0xd000:
        case 0xd400:
        case 0xd800:
        case 0xdc00:
            inst_rcall(avr, inst);
            break;
        case 0xe000:
        case 0xe400:
        case 0xe800:
        case 0xec00:
            inst_ldi(avr, inst);
            break;
        case 0xf000:
        case 0xf400:
            inst_branch(avr, inst);
            break;
        case 0xf800:
            if ((inst & 0x0200) == 0x0000) inst_bld(avr, inst);
            else inst_bst(avr, inst);
            break;
        case 0xfc00:
            if ((inst & 0x0008) == 0) {
                if ((inst & 0x0200) == 0x0000) inst_sbrc(avr, inst);
                else inst_sbrs(avr, inst);
            } else {
                goto invalid_instruction;
            }
            break;
        default:
            goto invalid_instruction;
            break;
    }

    return;

invalid_instruction:
    avr_panic(avr, AVR_INVALID_INSTRUCTION);
}
