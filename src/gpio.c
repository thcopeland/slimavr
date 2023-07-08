#include "gpio.h"

static enum avr_pin_state avr_pin_internal(struct avr *avr, uint8_t port, uint8_t pin) {
    if (avr->mem[avr->model.reg_ddrs[port]] & (1<<pin)) { // output
        if (avr->mem[avr->model.reg_ports[port]] & (1<<pin)) {
            return PIN_HIGH;
        }
        return PIN_LOW;
    } else { // input
        if (avr->mem[avr->model.reg_ports[port]] & (1<<pin)) {
            return PIN_PULLUP;
        }
        return PIN_FLOATING;
    }
}

const enum avr_pin_state avr_io_sample_lut[5][5] = {
    //  low,        high,       pulldown,   pullup,     floating
    {   PIN_LOW,    PIN_LOW,    PIN_LOW,    PIN_LOW,    PIN_LOW         },      // external PIN_LOW
    {   PIN_HIGH,   PIN_HIGH,   PIN_HIGH,   PIN_HIGH,   PIN_HIGH        },      // external PIN_HIGH
    {   PIN_LOW,    PIN_HIGH,   PIN_LOW,    PIN_LOW,    PIN_LOW         },      // external PIN_PULLDOWN
    {   PIN_LOW,    PIN_HIGH,   PIN_HIGH,   PIN_HIGH,   PIN_HIGH        },      // external PIN_PULLUP
    {   PIN_LOW,    PIN_HIGH,   PIN_LOW,    PIN_HIGH,   PIN_FLOATING    }       // external PIN_FLOATING
};

enum avr_pin_state avr_io_sample(struct avr *avr, uint8_t port, uint8_t pin) {
    if (port < avr->model.port_count && pin < 8 && avr->model.reg_ports[port] != 0) {
        enum avr_pin_state external = avr->pin_data[port][pin],
                           internal = avr_pin_internal(avr, port, pin);

        return avr_io_sample_lut[external][internal];
    } else {
        return PIN_FLOATING;
    }
}

uint8_t avr_io_sample_port(struct avr *avr, uint8_t port) {
    return ((avr_io_sample(avr, port, 0) == PIN_HIGH)) |
           ((avr_io_sample(avr, port, 1) == PIN_HIGH) << 1) |
           ((avr_io_sample(avr, port, 2) == PIN_HIGH) << 2) |
           ((avr_io_sample(avr, port, 3) == PIN_HIGH) << 3) |
           ((avr_io_sample(avr, port, 4) == PIN_HIGH) << 4) |
           ((avr_io_sample(avr, port, 5) == PIN_HIGH) << 5) |
           ((avr_io_sample(avr, port, 6) == PIN_HIGH) << 6) |
           ((avr_io_sample(avr, port, 7) == PIN_HIGH) << 7);
}

void avr_io_connect(struct avr *avr, uint8_t port, uint8_t pin, enum avr_pin_state value) {
    if (port < avr->model.port_count && pin < 8 && avr->model.reg_ports[port] != 0) {
        avr->pin_data[port][pin] = value;

        // TODO: interrupts, communication, etc
    }
}
