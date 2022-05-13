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
    .eep_pgsize     = 16,
    .flash_pgsize   = 256,
    .flash_bootsize = 8192,
    .flash_nrwwsize = 8192,
    .io_offset      = 0x20,
    .reg_rampz      = 0x5b,
    .reg_eind       = 0x5c,
    .reg_stack      = 0x5d,
    .reg_status     = 0x5f,
    .reg_spmcsr     = 0x57
};
