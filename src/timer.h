#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

enum avr_timer_type {
    TIMER_STANDARD,     // timers 1,3-5 for ATmega 640/1280/1281/2560/2561
    TIMER_REDUCED,      // timer 0,2 for ATmega 640/1280/1281/2560/2561
};

#define AVR_CS_DISABLED 0x0000
#define AVR_CS_FALLING 0x8000
#define AVR_CS_RISING 0x8001
#define AVR_CS_PRESCALE(val) (1<<val)

struct avr_timer {
    // timer information
    enum avr_timer_type type;
    uint8_t resolution;     // number of bits of resolution (8 and 16 supported)
    uint8_t comparators;    // number of comparators (up to 3)
    uint8_t sleep_mask;     // whether to continue during which sleep modes
    uint16_t clock_src[8];  // clock source array (prescaler, external clock)

    // timer control registers
    uint16_t reg_tcnt;      // the timer value register
    uint16_t reg_ocra;      // timer output compare register A
    uint16_t reg_ocrb;      // timer output compare register B
    uint16_t reg_ocrc;      // timer output compare register C
    uint8_t  reg_tmp16;     // (internal) temporary register to use for atomic 16 bit operations (0-31)
    uint16_t reg_tccra;     // timer/counter control register A
    uint16_t reg_tccrb;     // timer/counter control register B
    uint16_t reg_tccrc;     // timer/counter control register C
    uint16_t reg_oca;       // output compare port A
    uint16_t reg_ocb;       // output compare port B
    uint16_t reg_occ;       // output compare port C
    uint8_t  msk_oca;       // output compare pin A
    uint8_t  msk_ocb;       // output compare pin B
    uint8_t  msk_occ;       // output compare pin C
    // uint16_t reg_icp;       // input capture port (TODO)
    // uint8_t  msk_icp;       // input capture pin (TODO)
    uint16_t reg_icr;       // input capture register (only PWM timing supported)
    uint16_t reg_timsk;     // timer interrupt mask register
    uint8_t  msk_ocfiea;    // output compare interrupt enabled A
    uint8_t  msk_ocfieb;    // output compare interrupt enabled B
    uint8_t  msk_ocfiec;    // output compare interrupt enabled C
    uint8_t  msk_toie;      // timer overflow interrupt enabled
    uint16_t reg_tifr;      // timer interrupt flag register
    uint8_t  msk_ocfa;      // output compare flag A
    uint8_t  msk_ocfb;      // output compare flag B
    uint8_t  msk_ocfc;      // output compare flag C
    uint8_t  msk_tovf;      // timer overflow flag
    uint16_t reg_external;  // external clock source port
    uint8_t  msk_external;  // external clock source pin

    // interrupt vectors
    // uint16_t vec_capt;      // input capture interrupt vector (TODO)
    uint16_t vec_compa;     // compare A interrupt vector
    uint16_t vec_compb;     // compare B interrupt vector
    uint16_t vec_compc;     // compare C interrupt vector
    uint16_t vec_ovf;       // overflow interrupt vector
};

struct avr_timerstate {
    uint16_t ocra_buff;
    uint16_t ocrb_buff;
    uint16_t ocrc_buff;
    uint16_t prescale;
    int8_t direction;
    uint8_t val; // TODO rename
};

struct avr;

void avr_update_timers(struct avr *avr);

uint32_t avr_find_timer_interrupt(struct avr *avr);

#endif
