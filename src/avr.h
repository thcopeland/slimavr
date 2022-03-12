#ifndef AVR_H
#define AVR_H

#define EBADINST 1
#define EUNSUPPORTED 2
#define EBADADDR 3
#define EBADPADDR 4

struct avr {
    int error;  // current error type, if any
    int status; // processor state
    int model;  // processor model
    int pc;     // program counter
    long clock; // processor clock
    // unsigned char *eeprom; // unsupported
    unsigned char *rom;  // program memory
    unsigned char *mem;  // ram
};

struct avr *avr_init(int model);
void avr_step(struct avr *avr);
void avr_free(struct avr *avr);

#endif
