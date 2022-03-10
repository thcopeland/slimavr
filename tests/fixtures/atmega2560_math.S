.include "m2560def.inc"

.dseg 0x0000
results: .byte 16

.cseg 0x000000
    ldi ZL, low(results)
    ldi ZH, high(results)
    ldi r16, 53
    ldi r17, 101
    add r16, r17
    st Z+, r16
