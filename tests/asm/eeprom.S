.include "m1280def.inc"
.include "base.inc"
    jmp test_eeprom
.org ERDYaddr
    jmp eep_done

eep_done:
    ldi r20, 1
    reti

test_eeprom:
    ldi r16, 1
    sts status, r16
    ldi r16, low(1234)
    ldi r17, high(1234)
    out EEARL, r16
    out EEARH, r17
    ldi r16, 42
    out EEDR, r16
    sbi EECR, EEMPE
    sbi EECR, EEPE
wait1:
    sbic EECR, EEPE
    rjmp wait1
    in r16, EEDR
    cpi r16, 42
    breq test_complex
fail1:
    rjmp fail1

test_complex:
    ldi r16, 2
    sts status, r16
    ldi r16, low(42)
    ldi r17, high(42)
    out EEARL, r16
    out EEARH, r17
    sbi EECR, EERE
    in r17, EEDR
    cpi r17, 0xff
    brne fail2
    ; write only
    ldi r17, 42
    out EEDR, r17
    sbi EECR, EEPM1
    sbi EECR, EEMPE
    sbi EECR, EEPE
wait2_1:
    sbic EECR, EEPE
    rjmp wait2_1
    sbi EECR, EERE
    in r17, EEDR
    cpi r17, 42
    brne fail2
    ; write only again
    ldi r17, 15
    out EEDR, r17
    sbi EECR, EEMPE
    sbi EECR, EEPE
wait2_2:
    sbic EECR, EEPE
    rjmp wait2_2
    sbi EECR, EERE
    in r17, EEDR
    cpi r17, 42 & 15
    sts status, r17
    brne fail2
    ; erase only
    cbi EECR, EEPM1
    sbi EECR, EEPM0
    sbi EECR, EEMPE
    sbi EECR, EEPE
wait2_3:
    sbic EECR, EEPE
    rjmp wait2_3
    sbi EECR, EERE
    in r17, EEDR
    cpi r17, 0xff
    sts status, r17
    brne fail2
    ; write outside 4-cycle window
    ldi r17, 42
    out EEDR, r17
    cbi EECR, EEPM1
    cbi EECR, EEPM0
    sbi EECR, EEPE
    ldi r24, low(10000)
    ldi r25, high(10000)
wait2_4:
    sbiw r24, 1
    brne wait2_4
    sbi EECR, EERE
    in r17, EEDR
    cpi r17, 42
    breq fail2

    rjmp test_eeprom_interrupt
fail2:
    rjmp fail2

test_eeprom_interrupt:
    ldi r16, 3
    sts status, r16
    ldi r16, low(4321)
    ldi r17, high(4331)
    out EEARL, r16
    out EEARH, r17
    ldi r16, 42
    out EEDR, r16
    sbi EECR, EEMPE
    sbi EECR, EEPE
    sbi EECR, EERIE
    sei
wait3:
    cpi r20, 1
    brne wait3
    sts status, r1

end:
    rjmp end

