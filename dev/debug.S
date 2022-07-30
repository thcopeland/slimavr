.include "m1280def.inc"

; VGA timing information for 640x480 @ 60Hz on a 16Mhz processor
.equ VGA_HORIZONTAL_ACTIVE_PIXELS = 640
.equ VGA_HFRONT_PORCH_PIXELS = 16
.equ VGA_HSYNC_WIDTH_PIXELS = 96
.equ VGA_HBACK_PORCH_PIXELS = 48
.equ VGA_TOTAL_HORIZONTAL_PIXELS = (VGA_HORIZONTAL_ACTIVE_PIXELS + VGA_HFRONT_PORCH_PIXELS + VGA_HSYNC_WIDTH_PIXELS + VGA_HBACK_PORCH_PIXELS)

.equ VGA_VERTICAL_ACTIVE_LINES = 480
.equ VGA_VFRONT_PORCH_LINES = 11
.equ VGA_VSYNC_WIDTH_LINES = 2
.equ VGA_VBACK_PORCH_LINES = 31
.equ VGA_TOTAL_VERTICAL_LINES = (VGA_VERTICAL_ACTIVE_LINES + VGA_VFRONT_PORCH_LINES + VGA_VSYNC_WIDTH_LINES + VGA_VBACK_PORCH_LINES)

.equ HSYNC_PERIOD = 512
.equ HSYNC_SYNC_WIDTH = (HSYNC_PERIOD*VGA_HSYNC_WIDTH_PIXELS/VGA_TOTAL_HORIZONTAL_PIXELS)

.equ VSYNC_PRESCALER = 256
.equ VSYNC_PERIOD = (HSYNC_PERIOD/VSYNC_PRESCALER*VGA_TOTAL_VERTICAL_LINES)
.equ VSYNC_FRONT_PORCH = (HSYNC_PERIOD/VSYNC_PRESCALER*VGA_VFRONT_PORCH_LINES)
.equ VSYNC_SYNC_WIDTH = (HSYNC_PERIOD/VSYNC_PRESCALER*VGA_VSYNC_WIDTH_LINES)
.equ VSYNC_BACK_PORCH = (HSYNC_PERIOD/VSYNC_PRESCALER*VGA_VBACK_PORCH_LINES)

.equ DISPLAY_WIDTH = 120
.equ DISPLAY_HEIGHT = 66 ; may be reduced to 65
.equ FOOTER_HEIGHT = 6

.equ DISPLAY_VERTICAL_STRETCH = 5
.equ DISPLAY_CLK_TOP = (VSYNC_BACK_PORCH+1)
.equ DISPLAY_CLK_BOTTOM = (HSYNC_PERIOD*DISPLAY_VERTICAL_STRETCH*DISPLAY_HEIGHT/VSYNC_PRESCALER + DISPLAY_CLK_TOP)

; if the rows are vertically offset (the screen appears split horizontally),
; change this by a hundred or so. (probably unnecessary)
.equ VIRT_ADJUST = 0

; write an immediate to the given RAM address
; clobbers r18
.macro sti ; k, I
    ldi r18, @1
    sts @0, r18
.endm

; write an immediate word to the given RAM address
; clobbers r18 and r19
.macro stiw ; k, I
    ldi r18, low(@1)
    ldi r19, high(@1)
    sts @0+1, r19
    sts @0, r18
.endm

; compare an immediate word with the given registers
; rtmp can be rl
.macro cpiw ; rl, rh, I, rtmp
    cpi @0, low(@2)
    ldi @3, high(@2)
    cpc @1, @3
.endm

; rapidly write 12 pixels to the given port
; clobbers r0
.macro write_12_pixels ; port, X|Y|Z
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
    ld r0, @1+
    out @0, r0
.endm

.macro write_byte ; location, reg
    push r30
    sts @0, r1
    sts @0+1, r1
    sts @0+2, r1
    sts @0+3, r1
    sts @0+4, r1
    sts @0+5, r1
    sts @0+6, r1
    sts @0+7, r1
    ldi r30, 0xff
    sbrc @1, 7
    sts @0, r30
    sbrc @1, 6
    sts @0+1, r30
    sbrc @1, 5
    sts @0+2, r30
    sbrc @1, 4
    sts @0+3, r30
    sbrc @1, 3
    sts @0+4, r30
    sbrc @1, 2
    sts @0+5, r30
    sbrc @1, 1
    sts @0+6, r30
    sbrc @1, 0
    sts @0+7, r30
    ldi r30, 0x7
    sts @0+8, r30
    pop r30
.endm

.dseg
.org SRAM_START

framebuffer:        .byte DISPLAY_WIDTH*DISPLAY_HEIGHT
vid_fbuff_offset:   .byte 2
vid_row_repeat:     .byte 1
vid_work_complete:  .byte 1
clock:              .byte 2
status:             .byte 1

.cseg
.org 0x0000
    jmp main
.org OC1Aaddr
    jmp isr_loop
.org OC5Baddr
    jmp t_ocb
.org OVF5addr
    jmp t_ovf

t_ocb:
    in r0, SREG
    inc r22
    out SREG, r0
    reti

t_ovf:
    in r0, SREG
    inc r21
    out SREG, r0
    reti

main:
    clr r1
    clr r20
    clr r21
    cli

t_normal:
    ldi r16, 1
    sts status, r16
    sts TCCR5A, r1
    ldi r16, (1 << CS10)
    sts TCCR5B, r16
    sts TCNT5H, r1
    sts TCNT5L, r1
    ldi r16, (1 << TOIE5)
    sts TIMSK5, r16
    sei
wait1:
    inc r20
    cpi r21, 3
    brlo wait1
check1:
    cli
    sts TCCR5B, r1
    lds r16, TCNT5L
    lds r17, TCNT5H


;  write_byte framebuffer, r16 ; 0x17
;     write_byte framebuffer+DISPLAY_WIDTH, r17 ; 0x00
;     write_byte framebuffer+2*DISPLAY_WIDTH, r20 ; 0xf9
;     ldi r20, 0xaa
;     write_byte framebuffer+3*DISPLAY_WIDTH, r20

t_prescaled:
    clr r20
    clr r21
    ldi r16, 2
    sts status, r16
    ldi r16, 0
    sts TCCR5A, r16
    ldi r16, (1 << CS51)
    sts TCCR5B, r16
    sts TCNT5H, r1
    sts TCNT5L, r1
    ldi r16, (1 << TOIE5)
    sts TIMSK5, r16
    sei
wait2:
    inc r20
    cpi r21, 1
    brlo wait2
check2:
    cli
    sts TCCR5B, r1
    lds r16, TCNT5L
    lds r17, TCNT5H

    ; write_byte framebuffer, r16 ; 0x03
    ; write_byte framebuffer+DISPLAY_WIDTH, r17 ; 0x00
    ; write_byte framebuffer+2*DISPLAY_WIDTH, r20 ; 0xff
    ; ldi r20, 0xaa
    ; write_byte framebuffer+3*DISPLAY_WIDTH, r20


t_ctc:
    clr r20
    clr r21
    clr r22
    ldi r16, 3
    sts status, r16
    ldi r16, 0xff
    sts TIFR5, r16
    ldi r16, low(1234)
    ldi r17, high(1234)
    ldi r18, low(321)
    ldi r19, high(321)
    ldi r24, (1 << TOIE5) | (1 << OCIE5B)
    ldi r25, (1 << WGM51)
    ldi r26, (1 << CS50)
    sts TCNT5H, r1
    sts TCNT5L, r1
    sts OCR5AH, r17
    sts OCR5AL, r16
    sts OCR5BH, r19
    sts OCR5BL, r18
    sts TIMSK5, r24
    sts TCCR5A, r25
    sts TCCR5B, r26
    sei
wait3:
    inc r20
    cpi r22, 5
    brlo wait3
check3:
    cli
    sts TCCR5B, r1
    lds r16, TCNT5L
    lds r17, TCNT5H


    write_byte framebuffer, r16 ; 0x28
    write_byte framebuffer+DISPLAY_WIDTH, r17 ; 0x01
    write_byte framebuffer+2*DISPLAY_WIDTH, r20 ; 0x9a
    ldi r20, 0xaa
    write_byte framebuffer+3*DISPLAY_WIDTH, r20


vga:
    ldi r18, 0xFF
    ldi r19, 0x00
    out DDRA, r18   ; VGA image output
    out DDRB, r18   ; PB6 is VGA HSYNC
    out DDRE, r18   ; PE4 is VGA VSYNC
    out DDRC, r19   ; controls
    out PORTC, r18  ; pull-up (?)
    ; init timers
    ; halt all timers
    ldi r18, (1 << TSM) | (1 << PSRASY) | (1 << PSRSYNC)
    out GTCCR, r18
    ; HSYNC
    ; initialize timer 1 to fast PWM (pin PB6)
    sti TCCR1A, (1 << WGM10) | (1 << WGM11) | (1 << COM1B1) | (1 << COM1B0)
    sti TCCR1B, (1 << WGM12) | (1 << WGM13) | (1 << CS10)
    stiw OCR1AL, (HSYNC_PERIOD - 1)
    stiw OCR1BL, (HSYNC_PERIOD - HSYNC_SYNC_WIDTH - 1)
    sti TIMSK1, (1 << OCIE1A)
    ; VSYNC
    ; initialize timer 3 to fast PWM (pin PE4)
    sti TCCR3A, (1 << WGM30) | (1 << WGM31) | (1 << COM3B1) | (1 << COM3B0)
    sti TCCR3B, (1 << WGM32) | (1 << WGM33) | (1 << CS32)
    stiw OCR3AL, (VSYNC_PERIOD - 1)
    stiw OCR3BL, (VSYNC_PERIOD - VSYNC_SYNC_WIDTH - 1)
    ; synchronize timers
    stiw TCNT1L, (HSYNC_PERIOD - 1)
    stiw TCNT3L, (VSYNC_PERIOD - VSYNC_SYNC_WIDTH - 1 - 20*VIRT_ADJUST)
    ; release timers
    out GTCCR, r1
    sei
    ; enable IDLE sleep mode for reliable interrupt timing
    ldi r18, (1 << SE)
    out SMCR, r18
_main_stall:
    sleep
    rjmp _main_stall
isr_loop:
    lds r18, TCNT3L
    lds r19, TCNT3H
    cpiw r18, r19, DISPLAY_CLK_TOP, r20
    brpl _loop_active_test2
    rjmp _loop_work
_loop_active_test2:
    cpiw r18, r19, DISPLAY_CLK_BOTTOM, r20
    brlo _loop_active_screen
    rjmp _loop_work
_loop_active_screen:
    ; output a single row from the framebuffer as quickly as reasonably possible.
    lds XL, vid_fbuff_offset
    lds XH, vid_fbuff_offset+1
    lds r16, vid_row_repeat
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    write_12_pixels PORTA, X
    sts vid_work_complete, r1 ; reset working state (somewhat out of place here, but simple)
    out PORTA, r1
    dec r16
    brpl _loop_quick_work
    sts vid_fbuff_offset, XL
    sts vid_fbuff_offset+1, XH
    ldi r16, DISPLAY_VERTICAL_STRETCH-1
_loop_quick_work:
    sts vid_row_repeat, r16
    rjmp _loop_end
_loop_work:
    lds r18, vid_work_complete
    inc r18
    cpi r18, 1
    brne _loop_reset_render_state
    sts vid_work_complete, r18
    in r0, PORTB
    ldi r16, 0x80
    eor r0, r16
    out PORTB, r0
    lds r24, clock
    lds r25, clock+1
    adiw r24, 1
    sts clock, r24
    sts clock+1, r25
_loop_reenter:
_loop_reset_render_state:
    sti vid_row_repeat, DISPLAY_VERTICAL_STRETCH
    stiw vid_fbuff_offset, framebuffer
    sbi TIFR1, OCF1A ; clear any pending interrupts
_loop_end:
    reti

; avra debug.S && avrdude -p atmega1280 -c wiring -P /dev/ttyACM0 -b 115200 -D -U flash:w:debug.S.hex:i
; avra debug.S && avrdude -p atmega1280 -c arduino -P /dev/ttyUSB0 -b 57600 -D -U flash:w:debug.S.hex:i
