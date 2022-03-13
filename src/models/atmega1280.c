#include "../model.h"

const struct avr_model MODEL_ATMEGA1280 = {
    .romsize = 64*1024,
    .memsize = 8*1024,
    .pcsize = 2,

    .status_reg = 0x3f,
    .stack_reg = 0x3d
};
