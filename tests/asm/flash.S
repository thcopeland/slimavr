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
    rcall test_store
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

test_store:
    ldi ZL, byte3(2*store_data)
    out RAMPZ, ZL
    ldi ZL, low(2*store_data)
    ldi ZH, high(2*store_data)
    ldi r16, (1<<SPMEN) | (1<<PGERS)
    out SPMCSR, r16
    spm
    rcall test_store_wait
    lpm r16, Z
    cpi r16, 0xff
    brne test_store_end
    ldi r18, 42
    ldi r19, 43
    movw r0, r18
    ldi r16, (1<<SPMEN)
    out SPMCSR, r16
    .dw 0x95f8 ; AVRA doesn't currently support spm Z+, so write opcode
    rcall test_store_wait
    inc r1
    mov r0, r1
    inc r1
    out SPMCSR, r16
    .dw 0x95f8 ; spm Z+
    rcall test_store_wait
    clr r1
    sei
    ldi r16, (1<<SPMEN)|(1<<PGWRT)|(1<<SPMIE)
    out SPMCSR, r16
    clr r20
    spm
_spm_wait:
    tst r20
    breq _spm_wait

    ldi ZL, low(2*store_data)
    ldi ZH, high(2*store_data)
    lpm r16, Z+
    cpi r16, 42
    brne test_store_end
    lpm r16, Z+
    cpi r16, 43
    brne test_store_end
    lpm r16, Z+
    cpi r16, 44
    brne test_store_end
    lpm r16, Z+
    cpi r16, 45
    brne test_store_end
    sts status, r1
test_store_end:
    ret

test_store_wait:
    in r0, SPMCSR
    sbrc r0, SPMEN
    rjmp test_store_wait
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

.org 0xd00
far_data: .db 24, 42
