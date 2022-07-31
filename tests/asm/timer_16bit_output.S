.include "m1280def.inc"
.include "base.inc"

.macro wait ; time
    ldi r24, low(@0)
    ldi r25, high(@0)
wait_loop_%:
    sbiw r24, 1
    brne wait_loop_%
.endm

.macro reset_t0
    sts TCCR1B, r1
    sts TCCR1A, r1
    out PORTB, r1
    ldi r16, 0xff ; spike to visually separate tests
    sts TCNT1H, r16
    sts TCNT1L, r16
.endm

test_normal:
    clr r1
    ldi r16, 1
    sts status, r16
    ldi r16, low(1234)
    ldi r17, high(1234)
    sts OCR1AH, r17
    sts OCR1AL, r16
    ldi r16, low(54321)
    ldi r17, high(54321)
    sts OCR1BH, r17
    sts OCR1BL, r16
    ldi r18, (1 << COM1A0) | (1 << COM1B0) | (1 << COM1B1) ; toggle on A, set on B
    ldi r19, (1 << CS10)
    sts TCCR1A, r18
    sts TCCR1B, r19

    sbic PORTB, 5
    rjmp fail
    sbic PORTB, 6
    rjmp fail
    wait 6000
    sbis PORTB, 5
    rjmp fail
    sbic PORTB, 6
    rjmp fail
    wait 10000
    sbis PORTB, 5
    rjmp fail
    sbis PORTB, 6
    rjmp fail
    wait 1000
    sbic PORTB, 5
    rjmp fail
    sbis PORTB, 6
    rjmp fail
    wait 10000
    sbic PORTB, 5
    rjmp fail
    sbis PORTB, 6
    rjmp fail
    reset_t0
    sts status, r1

fail:
    rjmp fail
