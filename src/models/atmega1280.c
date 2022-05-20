#include "../model.h"

static struct avr_port ports[] = {
    { .reg_dd = 0x021, .reg_port = 0x022, .reg_pin = 0x020 }, // port A
    { .reg_dd = 0x024, .reg_port = 0x025, .reg_pin = 0x023 }, // port B
    { .reg_dd = 0x027, .reg_port = 0x028, .reg_pin = 0x026 }, // port C
    { .reg_dd = 0x02a, .reg_port = 0x02b, .reg_pin = 0x029 }, // port D
    { .reg_dd = 0x02d, .reg_port = 0x02e, .reg_pin = 0x02c }, // port E
    { .reg_dd = 0x030, .reg_port = 0x031, .reg_pin = 0x02f }, // port F
    { .reg_dd = 0x033, .reg_port = 0x034, .reg_pin = 0x032 }, // port G
    { .reg_dd = 0x101, .reg_port = 0x102, .reg_pin = 0x100 }, // port H
    { .reg_dd = 0x104, .reg_port = 0x105, .reg_pin = 0x103 }, // port J
    { .reg_dd = 0x107, .reg_port = 0x108, .reg_pin = 0x106 }, // port K
    { .reg_dd = 0x10a, .reg_port = 0x10b, .reg_pin = 0x109 }  // port L
};

static struct avr_timer timers[] = {
    {   // timer 0
        .type           = TIMER_REDUCED,
        .resolution     = 8,
        .comparators    = 2,
        .sleep_mask     = 0xff, // TODO
        .wgm_table      = { WGM_NORMAL, WGM_PHASE_PWM_8BIT, WGM_CLEAR_ON_COMPARE_OCRA, WGM_FAST_PWM_8BIT, WGM_RESERVED, WGM_PHASE_PWM_OCRA, WGM_RESERVED, WGM_FAST_PWM_OCRA },
        .clock_src_table = { CS_DISABLED, CS_1, CS_8, CS_64, CS_256, CS_1024, CS_FALLING, CS_RISING },
        .com_non_pwm_table = { COM_DISCONNECTED, COM_TOGGLE, COM_CLEAR, COM_SET },
        .com_fast_pwm_table = { COM_DISCONNECTED, COM_TOGGLE, COM_NON_INVERTING, COM_INVERTING },
        .com_phase_pwm_table = { COM_DISCONNECTED, COM_TOGGLE, COM_CLEAR_UP_SET_DOWN, COM_SET_UP_CLEAR_DOWN },
        .reg_tcnt       = 0x46,
        .reg_ocra       = 0x47,
        .reg_ocrb       = 0x48,
        .reg_ocrc       = 0,
        .reg_tmp16      = 0,
        .reg_tccra      = 0x44,
        .reg_tccrb      = 0x45,
        .reg_tccrc      = 0,
        .reg_oca        = 0x25,
        .reg_ocb        = 0x34,
        .reg_occ        = 0,
        .msk_oca        = 0x80,
        .msk_ocb        = 0x20,
        .msk_occ        = 0,
        .reg_icr        = 0,
        .reg_timsk      = 0x6e,
        .reg_external   = 0x02b,
        .msk_external   = 0x80,
        .msk_ociea      = 0x02,
        .msk_ocieb      = 0x04,
        .msk_ociec      = 0,
        .msk_toie       = 0x01,
        .reg_tifr       = 0x35,
        .msk_ocfa       = 0x02,
        .msk_ocfb       = 0x04,
        .msk_ocfc       = 0,
        .msk_tovf       = 0x01,
        .vec_compa      = 0x0054,
        .vec_compb      = 0x0058,
        .vec_compc      = 0,
        .vec_ovf        = 0x005c,
    }
};

static struct avr_register regmap[] = {
    // TODO
};

const struct avr_model MODEL_ATMEGA1280 = {
    .romsize        = 128*1024,
    .regsize        = 512,
    .ramsize        = 8*1024,
    .eepsize        = 4*1024,
    .ramstart       = 0x200,
    .romstart       = 0,
    .eepstart       = 0,
    .memend         = 0x2200,
    .pcsize         = 2,
    .eep_pgsize     = 16,
    .flash_pgsize   = 256,
    .flash_bootsize = 8192,
    .flash_nrwwsize = 8192,
    .io_offset      = 0x20,
    .interrupt_time = 5,
    .reg_rampz      = 0x5b,
    .reg_eind       = 0x5c,
    .reg_stack      = 0x5d,
    .reg_status     = 0x5f,
    .reg_spmcsr     = 0x57,
    .reg_mcucr      = 0x55,
    .port_count     = sizeof(ports)/sizeof(ports[0]),
    .ports          = ports,
    .timer_count    = sizeof(timers)/sizeof(timers[0]),
    .timers         = timers,
    .regmap         = regmap
};
