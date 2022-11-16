.include "m1280def.inc"
.include "base.inc"

.dseg

.cseg
    jmp main
.org OC0Aaddr
    jmp oc0a

    ; block stray interrupts
    reti
oc0a:
    adiw r24, 1
    reti

main:
    clr r1
    clr r24
    clr r25
    sts status, r1
    ldi r16, 200
    sts OCR0A+0x20, r16
    ldi r16, (1 << WGM01)
    sts TCCR0A+0x20, r16
    sts TCNT0+0x20, r1
    ldi r16, (1 << OCIE0A)
    sts TIMSK0, r16
    ldi r16, (1 << CS00)
    sts TCCR0B+0x20, r16
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
    sts TCCR0B+0x20, r1
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
