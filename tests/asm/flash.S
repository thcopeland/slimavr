.include "m1280def.inc"
.include "base.inc"
    jmp test_all
.org SPMRaddr
    jmp spm_done

spm_done:
    ldi r20, 1
    reti

test_all:
    ldi r16, 1
    sts status, r16
    rcall test_load
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 2
    sts status, r16
    rcall test_write
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 3
    sts status, r16
    rcall test_erase
    lds r16, status
    cpi r16, 0
    brne stall

stall:
    rjmp stall

test_load:
    ldi ZL, low(2*near_data)
    ldi ZH, high(2*near_data)
    lpm
    mov r16, r0
    cpi r16, 42
    brne test_load_end
    lpm r16, Z+
    cpi r16, 42
    brne test_load_end
    lpm r16, Z+
    cpi r16, 24
    brne test_load_end
    ldi ZL, byte3(2*far_data)
    out RAMPZ, ZL
    ldi ZL, low(2*far_data)
    ldi ZH, high(2*far_data)
    elpm
    mov r16, r0
    cpi r16, 24
    brne test_load_end
    elpm r16, Z+
    cpi r16, 24
    brne test_load_end
    elpm r16, Z+
    cpi r16, 42
    brne test_load_end
    sts status, r1
test_load_end:
    ret

test_write:
    ldi ZL, low(2*store_data)
    ldi ZH, high(2*store_data)
    ldi r16, 1 << SPMEN
    ldi r17, (1 << SPMEN) | (1 << PGWRT)
    ldi r18, 42
    ldi r19, 43
    movw r0, r18
    out SPMCSR, r16
    .dw 0x95f8 ; spm Z+
    ldi r18, 44
    ldi r19, 45
    movw r0, r18
    out SPMCSR, r16
    spm
    add r0, r1
    out SPMCSR, r16
    spm
    out SPMCSR, r17
    spm
_tw_wait:
    in r0, SPMCSR
    and r0, r16
    brne _tw_wait
    ldi ZL, low(2*store_data)
    ldi ZH, high(2*store_data)
    elpm r16, Z+
    elpm r17, Z+
    elpm r18, Z+
    elpm r19, Z+
    cpi r16, 42
    brne _test_write_end
    cpi r17, 43
    brne _test_write_end
    cpi r18, 44
    brne _test_write_end
    cpi r19, 45
    brne _test_write_end
    sei
    ldi ZL, low(2*store_data)
    ldi ZH, high(2*store_data)
    ldi r16, (1 << SPMEN) | (1 << PGWRT) | (1 << SPMIE)
    ldi r18, 123
    ldi r19, 234
    clr r20
    movw r0, r18
    out SPMCSR, r16
    spm
_tw_wait2:
    tst r20
    breq _tw_wait2
    cli
    clr r1
    sts status, r1
_test_write_end:
    ret

test_erase:
    ldi ZL, low(2*store_data)
    ldi ZH, high(2*store_data)
    ldi r16, (1 << SPMEN) | (1 << PGERS)
    out SPMCSR, r16
    spm
_te_wait:
    in r0, SPMCSR
    and r0, r16
    brne _te_wait
    elpm r16, Z+
    elpm r17, Z+
    elpm r18, Z+
    elpm r19, Z+
    cpi r16, 0xff
    brne _test_erase_end
    cpi r17, 0xff
    brne _test_erase_end
    cpi r18, 0xff
    brne _test_erase_end
    cpi r19, 0xff
    brne _test_erase_end
    sts status, r1
_test_erase_end:
    ret

.org 0x200
.dw 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
.dw 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
.dw 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
.dw 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
.dw 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

store_data: .db 42, 42, 42, 42

.org 0x800
near_data: .db 42, 24

.org 0xd000
far_data: .db 24, 42
