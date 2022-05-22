.include "m1280def.inc"
.include "base.inc"

.macro wait ; time
    ldi r16, @0
wait_loop_%:
    dec r16
    brne wait_loop_%
.endm

test_normal:
    clr r1
    ldi r16, 1
    sts status, r16
    ldi r16, 1 << 7
    out DDRB, r16
    ldi r16, 128
    ldi r17, 16
    ldi r18, (1 << COM0A0) | (1 << COM0B0) | (1 << COM0B1) ; toggle on A, set on B
    ldi r19, (1 << CS00)
    sts OCR0A+0x20, r16
    sts OCR0B+0x20, r17
    sts TCCR0A+0x20, r18
    sts TCCR0B+0x20, r19
    sbic PORTB, 7
    jmp fail
    sbic PORTG, 5
    jmp fail
    wait 31
    sbic PORTB, 7
    jmp fail
    sbis PORTG, 5
    jmp fail
    wait 31
    sbis PORTB, 7
    jmp fail
    sbis PORTG, 5
    jmp fail
    wait 31
    sbis PORTB, 7
    jmp fail
    sbis PORTG, 5
    jmp fail
    wait 31
    sbic PORTB, 7
    jmp fail
    sbis PORTG, 5
    jmp fail
    wait 31
    sbic PORTB, 7
    jmp fail
    sbis PORTG, 5
    jmp fail
    out PORTB, r1
    out PORTG, r1

test_ctc:
    ldi r16, 2
    sts status, r16
    ldi r16, 32
    ldi r18, (1 << COM0A0) | (1 << WGM01) ; toggle on A
    ldi r19, (1 << CS00)
    sts OCR0A+0x20, r16
    sts TCCR0A+0x20, r18
    sts TCCR0B+0x20, r19
    sbic PORTB, 7
    jmp fail
    wait 16
    sbis PORTB, 7
    jmp fail
    wait 10
    sbic PORTB, 7
    jmp fail
    wait 10
    sbis PORTB, 7
    jmp fail
    wait 10
    sbic PORTB, 7
    jmp fail
    wait 10
    sbis PORTB, 7
    jmp fail
    wait 10
    sbic PORTB, 7
    jmp fail
    wait 10
    sbis PORTB, 7
    jmp fail
    sts status, r1

fail:
    rjmp fail
