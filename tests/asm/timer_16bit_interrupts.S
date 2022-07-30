.include "m1280def.inc"
.include "base.inc"
    jmp main
.org OC1Baddr
    jmp t_ocb
.org OVF1addr
    jmp t_ovf

t_ocb:
    in r0, SREG
    inc r22
    out SREG, r0
    reti

t_ovf:
    in r0, SREG
    inc r21
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
    sts TCCR1A, r1
    ldi r16, (1 << CS10)
    sts TCCR1B, r16
    sts TCNT1H, r1
    sts TCNT1L, r1
    ldi r16, (1 << TOIE1)
    sts TIMSK1, r16
    sei
wait1:
    inc r20
    cpi r21, 3
    brlo wait1
check1:
    cli
    sts TCCR1B, r1
    lds r16, TCNT1L
    lds r17, TCNT1H
    cpi r16, 19 ; should be 23
    cpc r17, r1
    brne fail1
    cpi r20, 248 ; should be 249
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
    sts TCCR1A, r16
    ldi r16, (1 << CS11)
    sts TCCR1B, r16
    sts TCNT1H, r1
    sts TCNT1L, r1
    ldi r16, (1 << TOIE1)
    sts TIMSK1, r16
    sei
wait2:
    inc r20
    cpi r21, 1
    brlo wait2
check2:
    cli
    sts TCCR1B, r1
    lds r16, TCNT1L
    lds r17, TCNT1H
    cpi r16, 2 ; should be 3
    cpc r17, r1
    brne fail2
    cpi r20, 255
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
    sts TIFR1, r16
    ldi r16, low(1234)
    ldi r17, high(1234)
    ldi r18, low(321)
    ldi r19, high(321)
    ldi r24, (1 << TOIE1) | (1 << OCIE1B)
    ldi r25, (1 << WGM11)
    ldi r26, (1 << CS10)
    sts TCNT1H, r1
    sts TCNT1L, r1
    sts OCR1AH, r17
    sts OCR1AL, r16
    sts OCR1BH, r19
    sts OCR1BL, r18
    sts TIMSK1, r24
    sts TCCR1A, r25
    sts TCCR1B, r26
    sei
wait3:
    inc r20
    cpi r22, 5
    brlo wait3
check3:
    cli
    sts TCCR1B, r1
    lds r16, TCNT1L
    lds r17, TCNT1H
    cpi r16, 43 ; should be 40
    ldi r18, 1
    cpc r17, r18
    brne fail3
    cpi r20, 142 ; should be 154
    brne fail3
    rjmp pass
fail3:
    rjmp fail3

pass:
    sts status, r1
stall:
    rjmp stall