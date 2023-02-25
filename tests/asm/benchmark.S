.include "m1280def.inc"
.include "base.inc"

; A simple but reasonably intense benchmark program. Completion is signaled by
; setting SRAM 0x42 to 1.
;   - two timers
;   - one interrupt
;   - no sleeping
;   - few multicycle (overall cheaper than single-cycle) instructions

.cseg
    jmp main
.org OC0Aaddr
    jmp oc0a

    ; block stray interrupts
    reti
oc0a:
    subi r24, low(-1)
    sbci r25, high(-1)
    ldi r16, 71
    lsr r16
    asr r16
    add r16, r16
    in r14, TCNT0
    lds r14, TCNT1L
    lds r15, TCNT1H
    add r14, r15
    reti

main:
    clr r1
    clr r24
    clr r25
    sts status, r1
    ldi r16, 100
    out OCR0A, r16
    ldi r16, (1 << WGM01)
    out TCCR0A, r16
    out TCNT0, r1
    ldi r16, (1 << OCIE0A)
    sts TIMSK0, r16
    ldi r16, (1 << CS00)
    out TCCR0B, r16
    ldi r16, low(1234)
    ldi r17, high(1234)
    sts OCR1AH, r17
    sts OCR1AL, r16
    ldi r16, (1 << COM1A0)
    sts TCCR1A, r16
    ldi r16, (1 << WGM12) | (1 << CS12) | (1 << CS10)
    sts TCCR1B, r16
    sei
loop:
    sei
    rcall work
    cli
    ldi r26, low(50000)
    ldi r27, high(50000)
    cp r24, r26
    cpc r25, r27
    brlo loop
end:
    cli
    out TCCR0B, r1
    sts TCCR1B, r1
    ldi r16, 1
    sts status, r16
stall:
    rjmp stall

work:
    ldi r16, 123
    mov r17, r16
    neg r17
    fmulsu r16, r17
    movw r16, r0
    clr r1
    subi r16, 34
    add r16, r17
    ret
