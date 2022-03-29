.include "m2560def.inc"
.include "base.inc"

test_rjmp:
    ldi r31, 1
    sts status, r31
    rjmp test_jmp
test_rjmp_stall:
    rjmp test_rjmp_stall
test_jmp:
    ldi r31, 2
    sts status, r31
    rjmp test_ijmp
test_jmp_stall:
    rjmp test_jmp_stall
.org 0x400
test_jmp_stall2:
    rjmp test_jmp_stall2
test_ijmp:
    ldi r31, 3
    sts status, r31
    ldi ZL, low(test_eijmp)
    ldi ZH, high(test_eijmp)
    ijmp
test_ijmp_stall:
    rjmp test_ijmp_stall
.org 0x1000
test_ijmp_stall2:
    rjmp test_ijmp_stall2
test_eijmp:
    ldi r31, 4
    sts status, r31
    ldi ZL, byte3(done)
    out EIND, ZL
    ldi ZL, low(done)
    ldi ZH, high(done)
    eijmp
test_eijmp_stall:
    rjmp test_eijmp_stall
.org 0x8000
test_eijmp_stall2:
    rjmp test_eijmp_stall2
done:
    ldi r31, 0
    sts status, r31
done_stall:
    rjmp done_stall
