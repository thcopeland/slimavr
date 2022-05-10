#include "../model.h"

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
    .in_out_offset  = 0x20,
    .reg_rampx      = 0,
    .reg_rampy      = 0,
    .reg_rampz      = 0x5b,
    .reg_eind       = 0x5c,
    .reg_stack      = 0x5d,
    .reg_status     = 0x5f
};
