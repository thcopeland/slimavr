#include "inst.h"


#include <stdio.h>

void inst_add(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("add\n");
    avr->pc += 2;
}

void inst_adc(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("adc\n");
    avr->pc += 2;
}

void inst_adiw(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("adiw\n");
    avr->pc += 2;
}

void inst_sub(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sub\n");
    avr->pc += 2;
}

void inst_subi(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("subi\n");
    avr->pc += 2;
}

void inst_sbc(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbc\n");
    avr->pc += 2;
}

void inst_sbci(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbci\n");
    avr->pc += 2;
}

void inst_sbiw(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("sbiw\n");
    avr->pc += 2;
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
    (void) avr;
    (void) inst;
    printf("eor\n");
    avr->pc += 2;
}

void inst_com(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("com\n");
    avr->pc += 2;
}

void inst_neg(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("neg\n");
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
    (void) avr;
    (void) inst;
    printf("in\n");
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

void inst_clr(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("clr\n");
    avr->pc += 2;
}

void inst_ser(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("ser\n");
    avr->pc += 2;
}

void inst_mul(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("mul\n");
    avr->pc += 2;
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
    (void) avr;
    (void) inst;
    printf("rjmp\n");
    avr->pc += 2;
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
    (void) avr;
    (void) inst;
    printf("cp\n");
    avr->pc += 2;
}

void inst_cpc(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("cpc\n");
    avr->pc += 2;
}

void inst_cpi(struct avr *avr, uint16_t inst) {
    (void) avr;
    (void) inst;
    printf("cpi\n");
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
    (void) avr;
    (void) inst;
    printf("branch\n");
    avr->pc += 2;
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
    (void) avr;
    (void) inst;
    printf("ldi\n");
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
    (void) avr;
    (void) inst;
    printf("lds\n");
    avr->pc += 4;
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
    (void) avr;
    (void) inst;
    printf("sts\n");
    avr->pc += 4;
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
