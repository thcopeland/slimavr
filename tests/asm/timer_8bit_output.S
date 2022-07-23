.include "m1280def.inc"
.include "base.inc"

.macro wait ; time
    ldi r16, @0
wait_loop_%:
    dec r16
    brne wait_loop_%
.endm

.macro reset_t0
    sts TCCR0B+0x20, r1
    sts TCCR0A+0x20, r1
    out PORTB, r1
    out PORTG, r1
    ldi r16, 200 ; spike to visually separate tests
    sts TCNT0+0x20, r16
    sts TCNT0+0x20, r1
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
    reset_t0

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
    reset_t0

test_fast_pwm:
    ldi r16, 3
    sts status, r16

    ldi r16, 75
    ldi r17, 40
    ; fast pwm up to OCR0A, set OC0B on match, clear at 0
    ldi r18, (1 << COM0B0) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00)
    ldi r19, (1 << CS00) | (1 << WGM02)
    sts TCNT0+0x20, r1
    sts OCR0A+0x20, r16
    sts OCR0B+0x20, r17
    sts TCCR0A+0x20, r18
    sts TCCR0B+0x20, r19
    wait 15
    sbis PORTG, 5
    jmp fail
    wait 10
    sbic PORTG, 5
    jmp fail
    wait 10
    sbic PORTG, 5
    jmp fail
    wait 10
    sbis PORTG, 5
    jmp fail
    wait 10
    sbic PORTG, 5
    jmp fail
    wait 10
    sbis PORTG, 5
    jmp fail
    reset_t0

test_phase_correct_pwm:
    ldi r16, 4
    sts status, r16

    ldi r16, 64
    ldi r17, 50
    ; phase correct pwm up to OCR0A, clear OC0B on match when up-counting, set when down-counting
    ldi r18, (1 << COM0B1) | (1 << WGM00)
    ldi r19, (1 << CS00) | (1 << WGM02)
    sts TCNT0+0x20, r1
    sts OCR0A+0x20, r16
    sts OCR0B+0x20, r17
    sts TCCR0A+0x20, r18
    sts TCCR0B+0x20, r19

    wait 30
    sbic PORTG, 5
    jmp fail
    wait 10
    sbic PORTG, 5
    jmp fail
    wait 10
    sbis PORTG, 5
    jmp fail
    wait 10
    sbis PORTG, 5
    jmp fail
    wait 10
    sbis PORTG, 5
    jmp fail
    wait 15
    sbic PORTG, 5
    jmp fail
    wait 5
    sbis PORTG, 5
    jmp fail
    wait 10
    sbis PORTG, 5
    jmp fail

    reset_t0
    sts status, r1

fail:
    rjmp fail
