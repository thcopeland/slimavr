#include "../model.h"

const struct avr_model MODEL_ATMEGA1280 = {
    .romsize = 64*1024,
    .memsize = 8*1024,
    .ramstart = 0x200,
    .pcsize = 2,

    .in_out_offset = 0x20,
    .status_reg = 0x5f,
    .stack_reg = 0x5d
};
