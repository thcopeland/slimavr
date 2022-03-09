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
    // char *eeprom; // unsupported
    char *rom;  // program memory
    char *mem;  // ram
};

struct avr *avr_new(int model);
void avr_check(struct avr *avr);
void avr_free(struct avr *avr);

#endif
