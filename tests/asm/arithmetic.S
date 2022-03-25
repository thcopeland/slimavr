.include "m2560def.inc"
.include "base.inc"

test_addition:
    ldi r16, -3
    ldi r17, 131
    add r16, r17
    in r18, SREG
    cpi r16, 128
    brne fail_add
    cpi r17, 131
    brne fail_add
    cpi r18, 0x35
    brne fail_add
    ldi r16, 72
    ldi r17, 8
    add r16, r17
    in r18, SREG
    cpi r16, 80
    brne fail_add
    cpi r18, 0x20
    brne fail_add
    rjmp test_16bit_addition
fail_add:
    ldi r31, 1
    sts status, r31
    rjmp stall

test_16bit_addition:
    ldi r16, low(12345)
    ldi r17, high(12345)
    ldi r18, low(4321)
    ldi r19, high(4321)
    add r16, r18
    in r20, SREG
    adc r17, r19
    in r21, SREG
    ldi r22, low(12345+4321)
    ldi r23, high(12345+4321)
    cp r16, r22
    brne fail_16bit_add
    cp r17, r23
    brne fail_16bit_add
    cpi r20, 0x01
    brne fail_16bit_add
    cpi r21, 0x00
    brne fail_16bit_add
    ldi r24, low(12345)
    ldi r25, high(12345)
    adiw r24, 63
    in r20, SREG
    ldi r26, low(12345+63)
    ldi r27, high(12345+63)
    cp r24, r26
    brne fail_16bit_add
    cpc r25, r27
    brne fail_16bit_add
    cpi r20, 0x00
    brne fail_16bit_add
    rjmp test_subtraction
fail_16bit_add:
    ldi r31, 2
    sts status, r31
    rjmp stall

test_subtraction:
    ldi r16, 154
    ldi r17, 13
    sub r16, r17
    in r18, SREG
    cpi r16, 141
    brne fail_sub
    cpi r17, 13
    brne fail_sub
    cpi r18, 0x34
    brne fail_sub
    ldi r16, -13
    ldi r17, 120
    sub r16, r17
    in r18, SREG
    cpi r16, 123
    brne fail_sub
    cpi r17, 120
    brne fail_sub
    cpi r18, 0x38
    brne fail_sub
    ldi r16, 26
    subi r16, 105
    in r18, SREG
    cpi r16, low(26-105)
    brne fail_sub
    cpi r18, 0x15
    rjmp test_16bit_subtraction
fail_sub:
    ldi r31, 3
    sts status, r31
    rjmp stall

test_16bit_subtraction:
    ldi r16, low(12345)
    ldi r17, high(12345)
    ldi r18, low(1023)
    ldi r19, high(1023)
    sub r16, r18
    in r20, SREG
    sbc r17, r19
    in r21, SREG
    cpi r16, low(12345-1023)
    brne fail_16bit_sub
    cpi r17, high(12345-1023)
    brne fail_16bit_sub
    cpi r20, 0x21
    brne fail_16bit_sub
    cpi r21, 0x20
    brne fail_16bit_sub
    ldi r16, low(54321)
    ldi r17, high(54321)
    subi r16, low(1023)
    in r20, SREG
    sbci r17, high(1023)
    in r21, SREG
    cpi r16, low(54321-1023)
    brne fail_16bit_sub
    cpi r17, high(54321-1023)
    brne fail_16bit_sub
    cpi r20, 0x21
    brne fail_16bit_sub
    cpi r21, 0x14
    brne fail_16bit_sub
    ldi r24, low(23145)
    ldi r25, high(23145)
    sbiw r24, 63
    in r20, SREG
    ldi r26, low(23145-63)
    ldi r27, high(23145-63)
    cp r24, r26
    brne fail_16bit_sub
    cp r25, r27
    brne fail_16bit_sub
    cpi r20, 0x00
    brne fail_16bit_sub
    rjmp test_negation
fail_16bit_sub:
    ldi r31, 4
    sts status, r31
    rjmp stall

test_negation:
    ldi r16, 45
    neg r16
    in r20, SREG
    cpi r16, -45
    brne fail_neg
    cpi r20, 0x35
    brne fail_neg
    ldi r16, 128
    neg r16
    in r20, SREG
    cpi r16, 128
    brne fail_neg
    cpi r20, 0x0d
    brne fail_neg
    rjmp stall
fail_neg:
    ldi r31, 5
    sts status, r31
stall:
    rjmp stall
