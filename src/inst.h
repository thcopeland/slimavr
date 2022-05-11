#ifndef INST_H
#define INST_H

#include "avr.h"

void inst_add(struct avr *avr, uint16_t inst);
void inst_adc(struct avr *avr, uint16_t inst);
void inst_adiw(struct avr *avr, uint16_t inst);
void inst_sub(struct avr *avr, uint16_t inst);
void inst_subi(struct avr *avr, uint16_t inst);
void inst_sbc(struct avr *avr, uint16_t inst);
void inst_sbci(struct avr *avr, uint16_t inst);
void inst_sbiw(struct avr *avr, uint16_t inst);
void inst_and(struct avr *avr, uint16_t inst);
void inst_andi(struct avr *avr, uint16_t inst);
void inst_or(struct avr *avr, uint16_t inst);
void inst_ori(struct avr *avr, uint16_t inst);
void inst_eor(struct avr *avr, uint16_t inst);
void inst_com(struct avr *avr, uint16_t inst);
void inst_neg(struct avr *avr, uint16_t inst);
void inst_inc(struct avr *avr, uint16_t inst);
void inst_in(struct avr *avr, uint16_t inst);
void inst_dec(struct avr *avr, uint16_t inst);
void inst_mul(struct avr *avr, uint16_t inst);
void inst_muls(struct avr *avr, uint16_t inst);
void inst_mulsu(struct avr *avr, uint16_t inst);
void inst_fmul(struct avr *avr, uint16_t inst);
void inst_fmuls(struct avr *avr, uint16_t inst);
void inst_fmulsu(struct avr *avr, uint16_t inst);
void inst_rjmp(struct avr *avr, uint16_t inst);
void inst_ijmp(struct avr *avr, uint16_t inst);
void inst_eijmp(struct avr *avr, uint16_t inst);
void inst_jmp(struct avr *avr, uint16_t inst);
void inst_rcall(struct avr *avr, uint16_t inst);
void inst_icall(struct avr *avr, uint16_t inst);
void inst_eicall(struct avr *avr, uint16_t inst);
void inst_call(struct avr *avr, uint16_t inst);
void inst_ret(struct avr *avr, uint16_t inst);
void inst_reti(struct avr *avr, uint16_t inst);
void inst_cpse(struct avr *avr, uint16_t inst);
void inst_cp(struct avr *avr, uint16_t inst);
void inst_cpc(struct avr *avr, uint16_t inst);
void inst_cpi(struct avr *avr, uint16_t inst);
void inst_sbrc(struct avr *avr, uint16_t inst);
void inst_sbrs(struct avr *avr, uint16_t inst);
void inst_sbic(struct avr *avr, uint16_t inst);
void inst_sbis(struct avr *avr, uint16_t inst);
void inst_branch(struct avr *avr, uint16_t inst);
// void inst_brbs(struct avr *avr, uint16_t inst);
// void inst_brbc(struct avr *avr, uint16_t inst);
// void inst_breq(struct avr *avr, uint16_t inst);
// void inst_brne(struct avr *avr, uint16_t inst);
// void inst_brcs(struct avr *avr, uint16_t inst);
// void inst_brcc(struct avr *avr, uint16_t inst);
// void inst_brsh(struct avr *avr, uint16_t inst);
// void inst_brlo(struct avr *avr, uint16_t inst);
// void inst_brmi(struct avr *avr, uint16_t inst);
// void inst_brpl(struct avr *avr, uint16_t inst);
// void inst_brge(struct avr *avr, uint16_t inst);
// void inst_brlt(struct avr *avr, uint16_t inst);
// void inst_brhs(struct avr *avr, uint16_t inst);
// void inst_brhc(struct avr *avr, uint16_t inst);
// void inst_brts(struct avr *avr, uint16_t inst);
// void inst_brtc(struct avr *avr, uint16_t inst);
// void inst_brvs(struct avr *avr, uint16_t inst);
// void inst_brvc(struct avr *avr, uint16_t inst);
// void inst_brie(struct avr *avr, uint16_t inst);
// void inst_brid(struct avr *avr, uint16_t inst);
void inst_sbi(struct avr *avr, uint16_t inst);
void inst_cbi(struct avr *avr, uint16_t inst);
void inst_lsl(struct avr *avr, uint16_t inst);
void inst_lsr(struct avr *avr, uint16_t inst);
void inst_rol(struct avr *avr, uint16_t inst);
void inst_ror(struct avr *avr, uint16_t inst);
void inst_asr(struct avr *avr, uint16_t inst);
void inst_swap(struct avr *avr, uint16_t inst);
void inst_bset(struct avr *avr, uint16_t inst);
void inst_bclr(struct avr *avr, uint16_t inst);
void inst_bst(struct avr *avr, uint16_t inst);
void inst_bld(struct avr *avr, uint16_t inst);
// void inst_sec(struct avr *avr, uint16_t inst);
// void inst_clc(struct avr *avr, uint16_t inst);
// void inst_sen(struct avr *avr, uint16_t inst);
// void inst_cln(struct avr *avr, uint16_t inst);
// void inst_sez(struct avr *avr, uint16_t inst);
// void inst_clz(struct avr *avr, uint16_t inst);
// void inst_sei(struct avr *avr, uint16_t inst);
// void inst_cli(struct avr *avr, uint16_t inst);
// void inst_ses(struct avr *avr, uint16_t inst);
// void inst_cls(struct avr *avr, uint16_t inst);
// void inst_sev(struct avr *avr, uint16_t inst);
// void inst_clv(struct avr *avr, uint16_t inst);
// void inst_set(struct avr *avr, uint16_t inst);
// void inst_clt(struct avr *avr, uint16_t inst);
// void inst_seh(struct avr *avr, uint16_t inst);
// void inst_clh(struct avr *avr, uint16_t inst);
void inst_mov(struct avr *avr, uint16_t inst);
void inst_movw(struct avr *avr, uint16_t inst);
void inst_ldi(struct avr *avr, uint16_t inst);
void inst_ldx(struct avr *avr, uint16_t inst);
void inst_ldy(struct avr *avr, uint16_t inst);
void inst_ldz(struct avr *avr, uint16_t inst);
void inst_ldd(struct avr *avr, uint16_t inst);
void inst_lds(struct avr *avr, uint16_t inst);
void inst_stx(struct avr *avr, uint16_t inst);
void inst_sty(struct avr *avr, uint16_t inst);
void inst_stz(struct avr *avr, uint16_t inst);
void inst_std(struct avr *avr, uint16_t inst);
void inst_sts(struct avr *avr, uint16_t inst);
void inst_lpm(struct avr *avr, uint16_t inst);
void inst_elpm(struct avr *avr, uint16_t inst);
void inst_spm(struct avr *avr, uint16_t inst);
void inst_out(struct avr *avr, uint16_t inst);
void inst_push(struct avr *avr, uint16_t inst);
void inst_pop(struct avr *avr, uint16_t inst);
void inst_mcu(struct avr *avr, uint16_t inst);
void inst_nop(struct avr *avr, uint16_t inst);
void inst_sleep(struct avr *avr, uint16_t inst);
void inst_wdr(struct avr *avr, uint16_t inst);
void inst_break(struct avr *avr, uint16_t inst);

#endif
