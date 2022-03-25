.include "m2560def.inc"
.include "base.inc"

test_and:
    rjmp stall
fail_and:
    ldi r31, 1
    sts status, r31
    rjmp stall

stall:
    rjmp stall
