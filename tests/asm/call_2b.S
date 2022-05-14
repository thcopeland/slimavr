.include "m1280def.inc"
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
    call test_ret
    lds r31, status
    cpi r31, 0
    brne stall
stall:
    rjmp stall

.org 0x400

test_rcall:
    in XL, SPL
    in XH, SPH
    ldi r18, low(0x21fd)
    ldi r19, high(0x21fd)
    cp r18, XL
    cpc r19, XH
    brne test_rcall_end
    adiw XL, 1
    ld r17, X+
    ld r16, X+
    ldi r18, low(rcall_ret)
    ldi r19, high(rcall_ret)
    cp r16, r18
    cpc r17, r19
    brne test_rcall_end
    sts status, r1
test_rcall_end:
    ret

.org 0x1000

test_call:
    in XL, SPL
    in XH, SPH
    ldi r18, low(0x21fd)
    ldi r19, high(0x21fd)
    cp r18, XL
    cpc r19, XH
    brne test_call_end
    adiw XL, 1
    ld r17, X+
    ld r16, X+
    ldi r18, low(call_ret)
    ldi r19, high(call_ret)
    cp r16, r18
    cpc r17, r19
    brne test_call_end
    sts status, r1
test_call_end:
    ret

test_icall:
    in XL, SPL
    in XH, SPH
    ldi r18, low(0x21fd)
    ldi r19, high(0x21fd)
    cp r18, XL
    cpc r19, XH
    brne test_icall_end
    adiw XL, 1
    ld r17, X+
    ld r16, X+
    ldi r18, low(icall_ret)
    ldi r19, high(icall_ret)
    cp r16, r18
    cpc r17, r19
    brne test_icall_end
    sts status, r1
test_icall_end:
    ret

test_ret:
    ldi r16, low(test_ret_pass)
    push r16
    ldi r16, high(test_ret_pass)
    push r16
    ret
test_ret_fail:
    ret
test_ret_pass:
    sts status, r1
    ret
