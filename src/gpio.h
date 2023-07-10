#ifndef SLIMAVR_GPIO_H
#define SLIMAVR_GPIO_H

#include <stdint.h>

/*
 * Slimavr handles simple IO by (very roughly) simulating circuit connections.
 * The result is more complicated than a simple HIGH/LOW model, but also more
 * accurate and flexible.
 *
 * In practice, though, you can just use avr_io_write with AVR_PIN_LOW/
 * AVR_PIN_HIGH/AVR_PIN_FLOATING to write values to the microcontroller and
 * avr_io_read to read values, and things will work as expected.
 */

enum avr_pin_state {
    AVR_PIN_LOW,        // set or pulled low
    AVR_PIN_HIGH,       // set or pulled high
    AVR_PIN_PULLDOWN,   // pulled low through high impedance
    AVR_PIN_PULLUP,     // pulled high ground through high impedance
    AVR_PIN_FLOATING    // not connected
};

struct avr;

/*
 * Initialize all IO pins.
 */
void avr_io_init(struct avr *avr);

/*
 * Sample the voltage on the given pin. The result depends both on the simulated
 * microcontroller's internal state and previous calls to avr_io_write. If these
 * conflict, AVR_PIN_LOW/AVR_PIN_HIGH overrides AVR_PIN_PULLDOWN/AVR_PIN_PULLUP
 * overrides AVR_PIN_FLOATING, and avr_io_write takes precedence over the
 * simulated core. The justification is that the external circuit can sink/source
 * more current.
 *
 * Always returns one of AVR_PIN_LOW, AVR_PIN_HIGH, or AVR_PIN_FLOATING.
 * AVR_PIN_FLOATING is also returned if port or pin is out of range.
 */
enum avr_pin_state avr_io_read(const struct avr *avr, char port, uint8_t pin);

/*
 * Similar to avr_io_read, but treats AVR_PIN_FLOATING as AVR_PIN_LOW and packs
 * the pin data for a port into a single byte.
 */
uint8_t avr_io_read_port(const struct avr *avr, char port);

/*
 * Simulate connecting the given pin to a voltage source or disconnecting it.
 * The actual pin value also depends on the simulated microcontroller's inner
 * state.
 */
void avr_io_write(struct avr *avr, char port, uint8_t pin, enum avr_pin_state value);

#endif
