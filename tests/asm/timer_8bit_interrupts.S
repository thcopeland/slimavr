.include "m1280def.inc"
.include "base.inc"
    jmp main
.org OC0Baddr
    jmp t_ocb
.org OVF0addr
    jmp t_ovf

t_ocb:
    in r0, SREG
    inc r22
    lds r17, TCNT0+0x20
    out SREG, r0
    reti

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
    cpi r16, 21
    brne fail1
    cpi r20, 122 ; should be 123
    brne fail1
    rjmp t_prescaled
fail1:
    rjmp fail1

t_prescaled:
    clr r20
    clr r21
    ldi r16, 2
    sts status, r16
    ldi r16, 0
    sts TCCR0A+0x20, r16
    ldi r16, (1 << CS01)
    sts TCCR0B+0x20, r16
    sts TCNT0+0x20, r1
    ldi r16, (1 << TOIE0)
    sts TIMSK0, r16
    sei
wait2:
    lds r16, TCNT0+0x20
    inc r20
    cpi r21, 3
    brlo wait2
check2:
    cli
    sts TCCR0B+0x20, r1
    lds r16, TCNT0+0x20
    cpi r16, 2
    brne fail2
    cpi r20, 250 ; should be 251
    brne fail2
    rjmp t_ctc
fail2:
    rjmp fail2

t_ctc:
    clr r20
    clr r21
    clr r22
    ldi r16, 3
    sts status, r16
    ldi r16, 0xff
    sts TIFR0+0x20, r16
    ldi r16, 64
    ldi r17, 60
    ldi r18, (1 << TOIE0) | (1 << OCIE0B)
    ldi r19, (1 << WGM01)
    ldi r20, (1 << CS00)
    sts TCNT0+0x20, r1
    sts OCR0A+0x20, r16
    sts OCR0B+0x20, r17
    sts TIMSK0, r18
    sts TCCR0A+0x20, r19
    sts TCCR0B+0x20, r20
    sei
wait3:
    lds r16, TCNT0+0x20
    inc r20
    cpi r22, 5
    brlo wait3
check3:
    cli
    sts TCCR0B+0x20, r1
    lds r16, TCNT0+0x20
    cpi r16, 21 ; should be 17
    brne fail3
    cpi r20, 44 ; should be 45
    brne fail3
    sts status, r1
stall:
    rjmp stall
fail3:
    rjmp fail3
