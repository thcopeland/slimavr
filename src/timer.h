#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// determine how tccra/tccrb/tccrc are decoded
enum avr_timer_type {
    TIMER_STANDARD,     // timers 1,3-5 for ATmega 640/1280/1281/2560/2561
    TIMER_REDUCED,      // timer 0,2 for ATmega 640/1280/1281/2560/2561
};

enum avr_timer_wgm {
    WGM_RESERVED,               // do nothing

    // count from 0 to some value, setting/clearing/toggling the OCnx pin when
    // a compare match occurs
    WGM_NORMAL,                 // count from 0 to MAX
    WGM_CLEAR_ON_COMPARE_ICR,   // count to ICNn
    WGM_CLEAR_ON_COMPARE_OCRA,  // count to OCRnA

    // count from 0 to some value, setting/clearing/toggling the OCnx pin when
    // a compare match occurs, then clearing/setting at TOP or BOTTOM
    WGM_FAST_PWM_8BIT,          // reset at 0xff
    WGM_FAST_PWM_9BIT,          // reset at 0x1ff
    WGM_FAST_PWM_10BIT,         // reset at 0x3ff
    WGM_FAST_PWM_ICR,           // reset at ICRn
    WGM_FAST_PWM_OCRA,          // reset at OCRnA

    // count from 0 to some value, setting/clearing the OCnx pin when a compare
    // match occurs, then count down to 0, clearing/setting the OCnx pin. the OCRnx
    // registers are double buffered and updated at TOP.
    WGM_PHASE_PWM_8BIT,         // start counting down at 0xff
    WGM_PHASE_PWM_9BIT,         // start counting down at 0x1ff
    WGM_PHASE_PWM_10BIT,        // start counting down at 0x3ff
    WGM_PHASE_PWM_ICR,          // start counting down at ICRn
    WGM_PHASE_PWM_OCRA,         // start counting down at OCRnA

    // identical to phase correct PWM except that OCRnx registers are updated at
    // BOTTOM
    WGM_PHASE_FREQ_PWM_ICR,     // start counting down at ICRn
    WGM_PHASE_FREQ_PWM_OCRA,    // start counting down at OCRnA
};

enum avr_timer_cs {
    CS_DISABLED,    // timer disabled
    CS_1,           // tick every cycle
    CS_2,           // tick every other cycle
    CS_4,           // tick every 4 cycles
    CS_8,           // tick every 8 cycles
    CS_16,          // tick every 16 cycles
    CS_32,          // tick every 32 cycles
    CS_64,          // tick every 64 cycles
    CS_128,         // tick every 128 cycles
    CS_256,         // tick every 256 cycles
    CS_512,         // tick every 512 cycles
    CS_1024,        // tick every 1024 cycles
    CS_FALLING,     // tick on the falling edge of an external signal
    CS_RISING,      // tick on the rising edge of an external signal
};

enum avr_timer_com {
    COM_DISCONNECTED,       // don't change the ouput pin at all
    COM_TOGGLE,             // toggle the output pin on match
    COM_CLEAR,              // clear the output pin on match
    COM_SET,                // set the output pin on match

    // fast PWM only
    COM_NON_INVERTING,      // clear the output pin on match, then set it at 0
    COM_INVERTING,          // set the output pin on match, then clear it at 0

    // phase correct PWM only
    COM_CLEAR_UP_SET_DOWN,  // clear on match when up-counting, set when down-counting
    COM_SET_UP_CLEAR_DOWN,  // set on match when up-counting, clear when down-counting

    // internally used to handle special PWM cases (eg OCRnx == TOP/BOTTOM)
    COM_SPECIAL             // either 100% high, 100% low, or 50% duty cycle
};

struct avr_timer {
    // timer information
    enum avr_timer_type type;
    uint8_t resolution;     // number of bits of resolution (8 and 16 supported)
    uint8_t comparators;    // number of comparators (up to 3)

    // various configuration tables
    enum avr_timer_wgm wgm_table[16];           // waveform generation settings
    enum avr_timer_cs clock_src_table[8];       // clock source array (prescaler, external clock)
    enum avr_timer_com com_non_pwm_table[4];    // compare output pin settings for non PWM modes
    enum avr_timer_com com_fast_pwm_table[4];   // compare output pin settings for fast PWM modes
    enum avr_timer_com com_phase_pwm_table[4];  // compare output pin settings for phase correct PWM modes

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
    uint16_t reg_icr;       // input capture register (only PWM timing supported)
    uint16_t reg_timsk;     // timer interrupt mask register
    uint8_t  msk_ociea;     // output compare interrupt enabled A
    uint8_t  msk_ocieb;     // output compare interrupt enabled B
    uint8_t  msk_ociec;     // output compare interrupt enabled C
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
    uint16_t prescale_clock;
    uint8_t ocra_low;
    uint8_t ocra_high;
    uint8_t ocrb_low;
    uint8_t ocrb_high;
    uint8_t ocrc_low;
    uint8_t ocrc_high;
    uint8_t tmp;
    int8_t delta;
};

struct avr;

void timerstate_init(struct avr_timerstate *state);

void avr_update_timers(struct avr *avr);

uint32_t avr_find_timer_interrupt(struct avr *avr);

#endif
