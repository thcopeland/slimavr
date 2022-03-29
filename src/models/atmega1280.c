#include "../model.h"

const struct avr_model MODEL_ATMEGA1280 = {
    .romsize        = 128*1024,
    .memsize        = 8*1024,
    .ramstart       = 0x200,
    .pcsize         = 2,
    .in_out_offset  = 0x20,
    .reg_rampz      = 0x5b,
    .reg_eind       = 0x5c,
    .reg_stack      = 0x5d,
    .reg_status     = 0x5f
};
