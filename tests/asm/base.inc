.dseg
.byte 0x0042 ; padding
status: .byte 1

.cseg
init:
    clr r1
    sts status, r1
