#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

enum avr_timer_type {
    TIMER_STANDARD,     // timers 1-5 for ATmega 640/1280/1281/2560/2561
    TIMER_REDUCED,      // timer 0 for ATmega 640/1280/1281/2560/2561
};

struct avr_timer {
    // timer information
    enum avr_timer_type type;
    uint8_t resolution;     // number of bits of resolution (8 and 16 supported)
    uint8_t comparators;    // number of comparators (up to 3)
    uint8_t sleep_mask;     // whether to continue during which sleep modes

    // timer control registers
    uint16_t reg_tcnt;      // the timer value register
    uint16_t reg_ocra;      // timer output compare register A
    uint16_t reg_ocrb;      // timer output compare register B
    uint16_t reg_ocrc;      // timer output compare register C
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
    // uint16_t reg_icr;       // input capture register (TODO)
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
    uint8_t  reg_tmp16;     // which (internal) temporary register to use for atomic 16 bit operations (should be 0-3)

    // interrupt vectors
    // uint16_t vec_capt;      // input capture interrupt vector
    uint16_t vec_compa;     // compare A interrupt vector
    uint16_t vec_compb;     // compare B interrupt vector
    uint16_t vec_compc;     // compare C interrupt vector
    uint16_t vec_ovf;       // overflow interrupt vector
};

struct avr;

void avr_update_timers(struct avr *avr);

#endif
