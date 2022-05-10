.include "m1280def.inc"
.include "base.inc"

.dseg
data: .byte 10

.cseg
test_all:
    ldi r16, 1
    sts status, r16
    rcall test_basic
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 2
    sts status, r16
    rcall test_direct
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 3
    sts status, r16
    rcall test_regs
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 4
    sts status, r16
    rcall test_displacement
    lds r16, status
    cpi r16, 0
    brne stall

stall:
    rjmp stall

test_basic:
    ldi XL, low(data)
    ldi XH, high(data)
    ldi r16, 42
    st X+, r16
    inc r16
    st X+, r16
    inc r16
    st X, r16
    ldi YL, low(data+2)
    ldi YH, high(data+2)
    ld r16, Y
    cpi r16, 44
    brne test_basic_end
    ld r16, -Y
    cpi r16, 43
    brne test_basic_end
    ld r16, -Y
    cpi r16, 42
    brne test_basic_end
    lds r16, data
    cpi r16, 42
    brne test_basic_end
    ldi ZL, low(status+1)
    ldi ZH, high(status+1)
    st -Z, r1
test_basic_end:
    ret

test_direct:
    ldi r16, 101
    sts data, r16
    lds r17, data
    cp r16, r17
    brne test_direct_end
    subi r16, 2
    sts data+1, r16
    lds r17, data+1
    cp r16, r17
    brne test_direct_end
    ldi ZL, low(data)
    ldi ZH, high(data)
    ld r17, Z+
    cpi r16, 99
    brne test_direct_end
    sts status, r1
test_direct_end:
    ret

test_regs:
    ldi r16, 1
    ldi r17, 1
    ldi r18, 2
    ldi r19, 3
    ldi r20, 5
    ldi r21, 8
    ldi r22, 13
    ldi r23, 21
    ldi XL, 0
    ldi XH, 0
    ldi YL, 16
    ldi YH, 0
    ldi r25, 8
_tr_loop:
    ld r24, Y+
    st X+, r24
    dec r25
    brne _tr_loop
    cp r0, r16
    cpc r1, r17
    cpc r2, r18
    cpc r3, r19
    cpc r4, r20
    cpc r5, r21
    cpc r6, r22
    cpc r7, r23
    brne test_regs_end
    clr r1
    sts status, r1
test_regs_end:
    ret

test_displacement:
    ldi YL, low(data)
    ldi YH, high(data)
    ldi r16, 100
    std Y+0, r16
    inc r16
    std Y+1, r16
    inc r16
    std Y+2, r16
    ldd r17, Y+2
    cpi r17, 102
    brne test_displacement_end
    ldd r17, Y+1
    cpi r17, 101
    brne test_displacement_end
    ldd r17, Y+0
    cpi r17, 100
    brne test_displacement_end
    lds r17, data
    cpi r17, 100
    brne test_displacement_end
    lds r17, data+1
    cpi r17, 101
    brne test_displacement_end
    lds r17, data+2
    cpi r17, 102
    brne test_displacement_end
    sts status, r1
test_displacement_end:
    ret
