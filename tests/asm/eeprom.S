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
    breq test_eeprom_interrupt
fail1:
    rjmp fail1

test_eeprom_interrupt:
    ldi r16, 2
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
wait2:
    cpi r20, 1
    brne wait2
    sts status, r1

end:
    rjmp end

