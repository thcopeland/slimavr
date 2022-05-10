.include "m2560def.inc"
.include "base.inc"

test_calls:
    clr r1
    ldi r31, 1
    sts status, r31
    rcall test_rcall
rcall_ret:
    lds r31, status
    cpi r31, 0
    brne stall
    ldi r31, 2
    sts status, r31
    call test_call
call_ret:
    lds r31, status
    cpi r31, 0
    brne stall
    ldi r31, 3
    sts status, r31
    ldi r30, low(test_icall)
    ldi r31, high(test_icall)
    icall
icall_ret:
    lds r31, status
    cpi r31, 0
    brne stall
    ldi r31, 4
    sts status, r31
    ldi r30, byte3(test_eicall)
    out EIND, r30
    ldi r30, low(test_eicall)
    ldi r31, high(test_eicall)
    eicall
eicall_ret:
    lds r31, status
    cpi r31, 0
    brne stall
stall:
    rjmp stall

.org 0x400

test_rcall:
    in XL, SPL
    in XH, SPH
    ldi r18, low(0x21fc)
    ldi r19, high(0x21fc)
    cp r18, XL
    cpc r19, XH
    brne test_rcall_end
    adiw XL, 1
    ld r17, X+
    ld r16, X+
    ld r15, X+
    ldi r18, low(rcall_ret)
    ldi r19, high(rcall_ret)
    ldi r20, byte3(rcall_ret)
    cp r15, r18
    cpc r16, r19
    cpc r17, r20
    brne test_rcall_end
    sts status, r1
test_rcall_end:
    ret

.org 0x1000

test_call:
    in XL, SPL
    in XH, SPH
    ldi r18, low(0x21fc)
    ldi r19, high(0x21fc)
    cp r18, XL
    cpc r19, XH
    brne test_call_end
    adiw XL, 1
    ld r17, X+
    ld r16, X+
    ld r15, X+
    ldi r18, low(call_ret)
    ldi r19, high(call_ret)
    ldi r20, byte3(call_ret)
    cp r15, r18
    cpc r16, r19
    cpc r17, r20
    brne test_call_end
    sts status, r1
test_call_end:
    ret

test_icall:
    in XL, SPL
    in XH, SPH
    ldi r18, low(0x21fc)
    ldi r19, high(0x21fc)
    cp r18, XL
    cpc r19, XH
    brne test_icall_end
    adiw XL, 1
    ld r17, X+
    ld r16, X+
    ld r15, X+
    ldi r18, low(icall_ret)
    ldi r19, high(icall_ret)
    ldi r20, byte3(icall_ret)
    cp r15, r18
    cpc r16, r19
    cpc r17, r20
    brne test_icall_end
    sts status, r1
test_icall_end:
    ret

.org 0x15000

test_eicall:
    in XL, SPL
    in XH, SPH
    ldi r18, low(0x21fc)
    ldi r19, high(0x21fc)
    cp r18, XL
    cpc r19, XH
    brne test_eicall_end
    adiw XL, 1
    ld r17, X+
    ld r16, X+
    ld r15, X+
    ldi r18, low(eicall_ret)
    ldi r19, high(eicall_ret)
    ldi r20, byte3(eicall_ret)
    cp r15, r18
    cpc r16, r19
    cpc r17, r20
    brne test_eicall_end
    sts status, r1
test_eicall_end:
    ret
