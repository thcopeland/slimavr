.include "m2560def.inc"
.include "base.inc"

test_unsigned_multiplication:
    ldi r16, 43
    ldi r17, 193
    mul r16, r17
    ldi r18, low(43*193)
    ldi r19, high(43*193)
    in r20, SREG
    cp r0, r18
    cpc r1, r19
    brne fail_umul
    cpi r20, 0x00
    brne fail_umul
    ldi r16, 255
    ldi r17, 200
    mul r16, r17
    ldi r18, low(255*200)
    ldi r19, high(255*200)
    in r20, SREG
    cp r0, r18
    cpc r1, r19
    brne fail_umul
    cpi r20, 0x01
    brne fail_umul
    rjmp test_signed_multiplication
fail_umul:
    ldi r31, 1
    sts status, r31
    rjmp stall

test_signed_multiplication:
    ldi r16, -125
    ldi r17, 117
    muls r16, r17
    in r20, SREG
    ldi r18, low(-125*117)
    ldi r19, high(-125*117)
    cp r0, r18
    cpc r1, r19
    brne fail_muls
    cpi r20, 0x01
    brne fail_muls
    ldi r16, -107
    ldi r17, -43
    muls r16, r17
    in r20, SREG
    ldi r18, low(107*43)
    ldi r19, high(107*43)
    cp r0, r18
    cpc r1, r19
    brne fail_muls
    cpi r20, 0x00
    brne fail_muls
    rjmp test_signed_unsigned_multiplication
fail_muls:
    ldi r31, 2
    sts status, r31
    rjmp stall

test_signed_unsigned_multiplication:
    ldi r16, -113
    ldi r17, 216
    mulsu r16, r17
    in r20, SREG
    ldi r16, low(-113*216)
    ldi r17, high(-113*216)
    cp r0, r16
    cpc r1, r17
    brne fail_mulsu
    cpi r20, 0x01
    brne fail_mulsu
    ldi r16, 113
    ldi r17, 216
    mulsu r16, r17
    in r20, SREG
    ldi r16, low(113*216)
    ldi r17, high(113*216)
    cp r0, r16
    cpc r1, r17
    brne fail_mulsu
    cpi r20, 0x00
    brne fail_mulsu
    rjmp test_fract_unsigned_multiplication
fail_mulsu:
    ldi r31, 3
    sts status, r31
    rjmp stall

test_fract_unsigned_multiplication:
    ldi r16, 163
    ldi r17, 221
    fmul r16, r17
    in r20, SREG
    ldi r18, low((163*221)<<1)
    ldi r19, high((163*221)<<1)
    cp r0, r18
    cpc r1, r19
    brne fail_fmul
    cpi r20, 0x01
    brne fail_fmul
    rjmp test_fract_signed_multiplication
fail_fmul:
    ldi r31, 4
    sts status, r31
    rjmp stall

test_fract_signed_multiplication:
    ldi r16, -123
    ldi r17, -50
    fmuls r16, r17
    in r20, SREG
    ldi r18, low((123*50)<<1)
    ldi r19, high((123*50)<<1)
    cp r0, r18
    cpc r1, r19
    brne fail_fmuls
    cpi r20, 0x00
    brne fail_fmuls
    rjmp test_fract_signed_unsigned_multiplication
fail_fmuls:
    ldi r31, 5
    sts status, r31
    rjmp stall

test_fract_signed_unsigned_multiplication:
    ldi r16, -123
    ldi r17, 204
    fmulsu r16, r17
    in r20, SREG
    ldi r18, low((-123*204)<<1)
    ldi r19, high((-123*204)<<1)
    cp r0, r18
    cpc r1, r19
    brne fail_fmulsu
    cpi r20, 0x01
    brne fail_fmulsu
    rjmp stall
fail_fmulsu:
    ldi r31, 6
    sts status, r31
    rjmp stall

stall:
    rjmp stall
