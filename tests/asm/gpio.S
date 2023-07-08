.include "m2560def.inc"
.include "base.inc"

gpio_blink_out:
    ldi r25, 1
    sts status, r25
    sbi DDRA, 0     ; output on PA0
    cbi PORTA, 0
    ldi r25, 3
_gpio_blink_out_loop:
    sbi PORTA, 0
    nop
    nop
    nop
    nop
    cbi PORTA, 0
    nop
    nop
    dec r25
    brne _gpio_blink_out_loop
    nop
    nop
    rjmp gpio_blink_in

gpio_blink_in:
    ldi r25, 2
    sts status, r25
    cbi DDRA, 0     ; input on PA0
    sbi PORTA, 1    ; pull-down
    ldi r24, 100
    ldi r25, 0
_gpio_bi_loop:
    sbic PINA, 0
    rjmp _gpio_bi_rising
_gpio_bi_wait_rising:
    ldi r23, 10
_gpio_bi_wait_rising_loop:
    sbic PINA, 0
    rjmp _gpio_bi_rising
    dec r23
    brne _gpio_bi_wait_rising_loop
    rjmp _gpio_bi_done
_gpio_bi_rising:
    inc r25
    sbis PINA, 0
    rjmp _gpio_bi_next
_gpio_bi_wait_falling:
    ldi r23, 10
_gpio_bi_wait_falling_loop:
    sbis PINA, 0
    rjmp _gpio_bi_next
    dec r23
    brne _gpio_bi_wait_falling_loop
    rjmp _gpio_bi_done
_gpio_bi_next:
    dec r24
    brne _gpio_bi_loop
_gpio_bi_done:
    cpi r25, 3
    brne stall
    sts status, r1

stall:
    rjmp stall
