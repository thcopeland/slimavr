#include "avr.h"
#include "gpio.h"

void avr_io_init(struct avr *avr) {
    for (unsigned i = 0; i < avr->model.port_count; i++) {
        for (unsigned j = 0; j < sizeof(avr->pin_data[0])/sizeof(avr->pin_data[0][0]); j++) {
            avr->pin_data[i][j] = AVR_PIN_FLOATING;
        }
    }
}

static enum avr_pin_state avr_pin_internal(const struct avr *avr, uint8_t port, uint8_t pin) {
    if (avr->mem[avr->model.reg_ddrs[port]] & (1<<pin)) { // output
        if (avr->mem[avr->model.reg_ports[port]] & (1<<pin)) {
            return AVR_PIN_HIGH;
        }
        return AVR_PIN_LOW;
    } else { // input
        if (avr->mem[avr->model.reg_ports[port]] & (1<<pin)) {
            return AVR_PIN_PULLUP;
        }
        return AVR_PIN_FLOATING;
    }
}

enum avr_pin_state avr_io_read(const struct avr *avr, char port, uint8_t pin) {
    uint8_t port_num = port - 'A';

    if (port_num < avr->model.port_count && pin < 8 && avr->model.reg_ports[port_num] != 0) {
        enum avr_pin_state external = avr->pin_data[port_num][pin],
                           internal = avr_pin_internal(avr, port_num, pin);

        if (external == AVR_PIN_HIGH || external == AVR_PIN_LOW) {
            return external;
        } else if (internal == AVR_PIN_HIGH || internal == AVR_PIN_LOW) {
            return internal;
        } else if (external == AVR_PIN_PULLDOWN) {
            return AVR_PIN_LOW;
        } else if (external == AVR_PIN_PULLUP) {
            return AVR_PIN_HIGH;
        } else if (internal == AVR_PIN_PULLDOWN) {
            return AVR_PIN_LOW;
        } else if (internal == AVR_PIN_PULLUP) {
            return AVR_PIN_HIGH;
        } else {
            return AVR_PIN_FLOATING;
        }
    } else {
        return AVR_PIN_FLOATING;
    }
}

uint8_t avr_io_read_port(const struct avr *avr, char port) {
    return ((avr_io_read(avr, port, 0) == AVR_PIN_HIGH)) |
           ((avr_io_read(avr, port, 1) == AVR_PIN_HIGH) << 1) |
           ((avr_io_read(avr, port, 2) == AVR_PIN_HIGH) << 2) |
           ((avr_io_read(avr, port, 3) == AVR_PIN_HIGH) << 3) |
           ((avr_io_read(avr, port, 4) == AVR_PIN_HIGH) << 4) |
           ((avr_io_read(avr, port, 5) == AVR_PIN_HIGH) << 5) |
           ((avr_io_read(avr, port, 6) == AVR_PIN_HIGH) << 6) |
           ((avr_io_read(avr, port, 7) == AVR_PIN_HIGH) << 7);
}

void avr_io_write(struct avr *avr, char port, uint8_t pin, enum avr_pin_state value) {
    uint8_t port_num = port - 'A';
    if (port_num < avr->model.port_count && pin < 8 && avr->model.reg_ports[port_num] != 0) {
        avr->pin_data[port_num][pin] = value;

        // TODO: interrupts, communication, etc
    }
}
