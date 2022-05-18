.include "m1280def.inc"
.include "base.inc"
    jmp main

.org OVF0addr
    jmp t_ovf

t_ovf:
    in r0, SREG
    inc r21
    lds r17, TCNT0+0x20
    out SREG, r0
    reti

main:
    clr r1
    clr r20
    clr r21
    cli

t_normal:
    ldi r16, 1
    sts status, r16
    ldi r16, 0
    sts TCCR0A+0x20, r16    ; + 0x20 to convert from IO address to standard address
    ldi r16, (1 << CS00)
    sts TCCR0B+0x20, r16
    sts TCNT0+0x20, r1
    ldi r16, (1 << TOIE0)
    sts TIMSK0, r16
    sei
wait1:
    lds r16, TCNT0+0x20
    inc r20
    cpi r21, 3
    brlo wait1
check1:
    cli
    sts TCCR0B+0x20, r1
    lds r16, TCNT0+0x20
    cpi r16, 21 ; approximate
    brne fail1
    cpi r20, 122 ; approximate
    brne fail1
    rjmp t_prescaled
fail1:
    rjmp fail1

t_prescaled:
    sts status, r1 ; REMOVE ME
    rjmp t_prescaled
