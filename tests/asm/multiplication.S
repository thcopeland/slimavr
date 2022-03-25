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
    rjmp stall
fail_umul:
    ldi r31, 4
    sts status, r31

stall:
    rjmp stall
