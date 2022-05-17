#ifndef PORT_H
#define PORT_H

#include <stdint.h>

struct avr_port {
    uint16_t reg_dd;        // data direction register
    uint16_t reg_port;      // pin output register
    uint16_t reg_pin;       // pin input register
};

// struct avr;
//
// int port_handle_change(struct avr *avr, uint16_t reg);

#endif
