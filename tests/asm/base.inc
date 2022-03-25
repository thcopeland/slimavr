.dseg
.byte 0x0042 ; padding
status: .byte 1

.cseg 0x000000
init:
    clr r1
    sts status, r1
