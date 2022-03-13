.include "m2560def.inc"

.dseg 0x0000
results: .byte 16

.cseg 0x000000
    nop
    movw r16, r18
    muls r16, r18
    mulsu r16, r18
    fmul r16, r18
    fmulsu r16, r18
    fmuls  r16, r18
    cpc r16, r18
    cp r16, r18
    sbc r16, r18
    sub r16, r18
    add r16, r18
    adc r16, r18
    and r16, r18
    eor r16, r18
    or r16, r18
    cpi r16, 16
    subi r16, 6
    sbci r16, 7
    andi r16, 0xf
    ori r16, 0xf
    std Y+10, r26
    ldd r26, Y+10
    sts 20, r16
    st Y+, r16
    st Z+, r18
    push r26
    lds r16, 20
    ld  r16, Y+
    ld  r18, Z+
    pop r26
    lpm r26, Z
    elpm r26, Z+
    com r16
    neg r16
    swap r16
    inc r16
    asr r16
    lsr r16
    ror r16
    clv
    ret
    reti
    sleep
    ; break
    ; wdr
    lpm
    elpm
    spm
    ijmp
    icall
    eijmp
    eicall
    dec r3
    jmp 300
    call 200
    adiw r24, 2
    sbiw r24, 2
    cbi PINA, 7
    sbi PINA, 7
    sbis PINA, 6
    sbic PINA, 5
    mul r24, r25
    in r2, PINA
    out DDRA, r2
    rjmp 100
    rcall 100
    brlo 10
    bld r26, 2
    bst r26, 3
    sbrc r26, 2
    sbrs r26, 1
    ldi ZL, low(results)
    ldi ZH, high(results)
    ldi r16, 53
    ldi r17, 101
    add r16, r17
    st Z+, r16
