.include "m2560def.inc"

.dseg
.byte 0x0042 ; padding
ok: .byte 1

.cseg 0x000000
    ; init
    clr r1
    sts ok, r1

    ; test addition
    ldi r16, -3
    ldi r17, 131
    add r16, r17
    in r18, SREG
    cpi r16, 128
    brne fail
    cpi r17, 131
    brne fail
    cpi r18, 0x35
    brne fail
    ldi r16, 72
    ldi r17, 8
    add r16, r17
    in r18, SREG
    cpi r16, 80
    brne fail
    cpi r18, 0x20
    brne fail

    ; test subtraction
    ldi r16, 154
    ldi r17, 13
    sub r16, r17
    in r18, SREG
    cpi r16, 141
    brne fail
    cpi r17, 13
    brne fail
    cpi r18, 0x34
    brne fail
    ldi r16, -13
    ldi r17, 120
    sub r16, r17
    in r18, SREG
    cpi r16, 123
    brne fail
    cpi r17, 120
    brne fail
    cpi r18, 0x38
    brne fail

    ; test multiplication
    ldi r16, 43
    ldi r17, 193
    mul r16, r17
    ldi r18, low(43*193)
    ldi r19, high(43*193)
    in r20, SREG
    cp r0, r18
    cpc r1, r19
    brne fail
    cpi r20, 0x00
    brne fail
    ldi r16, 255
    ldi r17, 200
    mul r16, r17
    ldi r18, low(255*200)
    ldi r19, high(255*200)
    in r20, SREG
    cp r0, r18
    cpc r1, r19
    brne fail
    cpi r20, 0x01
    brne fail

stall:
    rjmp stall
fail:
    ser r31
    sts ok, r31
    rjmp stall
