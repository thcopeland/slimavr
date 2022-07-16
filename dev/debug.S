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
.org OC0Aaddr
    jmp t_oca

t_oca:
    lds r16, TCNT0+0x20
    in ZL, SPL
    in ZH, SPH
    ldd r17, Z+2
    sts TCCR0B, r1
    reti

main:
    sei
;     ldi r16, 7
;     ldi r17, (1 << OCIE0A)
;     ldi r18, (1 << CS00)
;     sts TCNT0+0x20, r1
;     sts OCR0A+0x20, r16
;     sts TCCR0A+0x20, r1
;     sts TIMSK0, r17
;     sts TCCR0B+0x20, r18
; nops: ; 0x42
;     nop ; 1
;     nop ; 2
;     nop ; 3
;     nop ; 4
;     nop ; 5
;     nop ; 6
;     nop ; 7
; ; expected return address <--- 0x49
;     nop
;     nop
; ; actual return address <----- 0x4b
;     nop
;     nop
;     nop
; ; at this point, r16=16, r17=0x4b
;     ; x: rcall x
    ldi r16, 45
    sts OCR0A, r16
    clr r16
    lds r16, OCR0A ; 45
    write_byte framebuffer, r16

    sti TCCR0A, (1<<WGM00)
    sti TCCR0B, 0
    ldi r16, 45
    sts OCR0A, r16
    clr r16
    lds r16, OCR0A ; 45
    write_byte framebuffer+DISPLAY_WIDTH, r16

    ldi r16, 45
    sts OCR5AH, r16
    clr r16
    lds r16, OCR5AH ; 0
    write_byte framebuffer+2*DISPLAY_WIDTH, r16

    ldi r16, 45
    sts OCR5AH, r16
    inc r16
    sts OCR5AL, r16
    clr r16
    lds r16, OCR5AL ; 46
    lds r16, OCR5AH ; 45
    write_byte framebuffer+3*DISPLAY_WIDTH, r16

    ; write_byte framebuffer+DISPLAY_WIDTH*2, r17 ; 0x4b
    ; write_byte framebuffer+DISPLAY_WIDTH*4, r20
    ; write_byte framebuffer+DISPLAY_WIDTH*6, r21

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
