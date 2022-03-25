.include "m2560def.inc"
.include "base.inc"

test_and:
    ldi r16, 0xfa
    ldi r17, 0xaf
    and r16, r17
    in r20, SREG
    cpi r16, 0xaa
    brne fail_and
    cpi r20, 0x14
    brne fail_and
    ldi r16, 0x47
    andi r16, 0x84
    in r20, SREG
    cpi r16, 0x04
    brne fail_and
    cpi r20, 0x00
    brne fail_and
    rjmp test_or
fail_and:
    ldi r31, 1
    sts status, r31
    rjmp stall

test_or:
    ldi r16, 0xfa
    ldi r17, 0xaf
    or r16, r17
    in r20, SREG
    cpi r16, 0xff
    brne fail_or
    cpi r20, 0x14
    brne fail_or
    ldi r16, 0x47
    ori r16, 0x84
    in r20, SREG
    cpi r16, 0xc7
    brne fail_or
    cpi r20, 0x14
    brne fail_or
    rjmp test_eor
fail_or:
    ldi r31, 2
    sts status, r31
    rjmp stall

test_eor:
    ldi r16, 0x17
    ldi r17, 0x16
    eor r16, r17
    in r20, SREG
    cpi r16, 0x01
    brne fail_eor
    cpi r20, 0x00
    brne fail_eor
    eor r16, r16
    in r20, SREG
    cpi r16, 0
    brne fail_eor
    cpi r20, 0x02
    brne fail_eor
    rjmp stall
fail_eor:
    ldi r31, 3
    sts status, r31
    rjmp stall

stall:
    rjmp stall
