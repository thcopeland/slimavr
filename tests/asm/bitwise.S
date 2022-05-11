.include "m2560def.inc"
.include "base.inc"

test_all:
    ldi r16, 1
    sts status, r16
    rcall test_and
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 2
    sts status, r16
    rcall test_or
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 3
    sts status, r16
    rcall test_eor
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 4
    sts status, r16
    rcall test_com
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 5
    sts status, r16
    rcall test_status
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 6
    sts status, r16
    rcall test_io
    lds r16, status
    cpi r16, 0
    brne stall

    ldi r16, 7
    sts status, r16
    rcall test_skips
    lds r16, status
    cpi r16, 0
    brne stall
stall:
    rjmp stall

test_and:
    ldi r16, 0xfa
    ldi r17, 0xaf
    and r16, r17
    in r20, SREG
    cpi r16, 0xaa
    brne test_and_end
    cpi r20, 0x14
    brne test_and_end
    ldi r16, 0x47
    andi r16, 0x84
    in r20, SREG
    cpi r16, 0x04
    brne test_and_end
    cpi r20, 0x00
    brne test_and_end
    sts status, r1
test_and_end:
    ret

test_or:
    ldi r16, 0xfa
    ldi r17, 0xaf
    or r16, r17
    in r20, SREG
    cpi r16, 0xff
    brne test_or_end
    cpi r20, 0x14
    brne test_or_end
    ldi r16, 0x47
    ori r16, 0x84
    in r20, SREG
    cpi r16, 0xc7
    brne test_or_end
    cpi r20, 0x14
    brne test_or_end
    sts status, r1
test_or_end:
    ret

test_eor:
    ldi r16, 0x17
    ldi r17, 0x16
    eor r16, r17
    in r20, SREG
    cpi r16, 0x01
    brne test_eor_end
    cpi r20, 0x00
    brne test_eor_end
    eor r16, r16
    in r20, SREG
    cpi r16, 0
    brne test_eor_end
    cpi r20, 0x02
    brne test_eor_end
    sts status, r1
test_eor_end:
    ret

test_com:
    ldi r16, 0x17
    com r16
    in r17, SREG
    cpi r16, ~0x17
    brne test_com_end
    cpi r17, 0x15
    brne test_com_end
    ldi r16, 0xff
    com r16
    in r17, SREG
    cpi r16, 0
    brne test_com_end
    cpi r17, 0x03
    brne test_com_end
    sts status, r1
test_com_end:
    ret

test_status:
    sei
    clt
    seh
    cls
    sev
    cln
    sez
    clc
    in r16, SREG
    cpi r16, 0xaa
    brne test_status_end
    sts status, r1
test_status_end:
    ret

test_io:
    sbi PORTC, 7
    cbi PORTC, 6
    sbi PORTC, 5
    cbi PORTC, 4
    sbi PORTC, 3
    cbi PORTC, 2
    sbi PORTC, 1
    cbi PORTC, 0
    sbis PORTC, 7
    rjmp test_io_end
    sbic PORTC, 6
    rjmp test_io_end
    sbis PORTC, 5
    rjmp test_io_end
    sbic PORTC, 4
    rjmp test_io_end
    sbis PORTC, 3
    rjmp test_io_end
    sbic PORTC, 2
    rjmp test_io_end
    sbis PORTC, 1
    rjmp test_io_end
    sbic PORTC, 0
    jmp test_io_end
    ; rjmp test_io_end
    sts status, r1
test_io_end:
    ret

test_skips:
    ldi r16, 0x0a
    sbrc r16, 7
    rjmp test_skips_end
    sbrc r16, 6
    rjmp test_skips_end
    sbrc r16, 5
    rjmp test_skips_end
    sbrc r16, 4
    rjmp test_skips_end
    sbrs r16, 3
    rjmp test_skips_end
    sbrc r16, 2
    rjmp test_skips_end
    sbrs r16, 1
    rjmp test_skips_end
    sbrc r16, 0
    rjmp test_skips_end
    out PORTC, r16
    sbic PORTC, 7
    rjmp test_skips_end
    sbic PORTC, 6
    rjmp test_skips_end
    sbic PORTC, 5
    rjmp test_skips_end
    sbic PORTC, 4
    rjmp test_skips_end
    sbis PORTC, 3
    rjmp test_skips_end
    sbic PORTC, 2
    rjmp test_skips_end
    sbis PORTC, 1
    rjmp test_skips_end
    sbic PORTC, 0
    lds r16, status
    sts status, r1
test_skips_end:
    ret
