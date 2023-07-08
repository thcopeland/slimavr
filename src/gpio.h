#ifndef SLIMAVR_GPIO_H
#define SLIMAVR_GPIO_H

#include <stdint.h>

/*
 * Slimavr handles simple IO by (very roughly) simulating circuit connections.
 * The result is more complicated than a simple HIGH/LOW model, but also more
 * accurate and flexible.
 *
 * In practice, though, you can just use avr_io_connect with PIN_LOW/PIN_HIGH/
 * PIN_FLOATING to write values to the microcontroller and avr_io_sample to
 * read values, and things will work as expected.
 */

enum avr_pin_state {
    PIN_LOW,        // set or pulled low
    PIN_HIGH,       // set or pulled high
    PIN_PULLDOWN,   // pulled low through high impedance
    PIN_PULLUP,     // pulled high ground through high impedance
    PIN_FLOATING    // not connected
};

struct avr;

void avr_io_init(struct avr *avr);

/*
 * Sample the voltage on the given pin. The result depends both on the simulated
 * microcontroller's internal state and previous calls to avr_io_write. If these
 * conflict, PIN_LOW/PIN_HIGH overrides PIN_PULLDOWN/PIN_PULLUP overrides
 * PIN_FLOATING, and avr_io_write takes precedence over the simulated core. The
 * justification is that the external circuit can sink/source more current.
 *
 * Always returns one of PIN_LOW, PIN_HIGH, or PIN_FLOATING. PIN_FLOATING is
 * also returned if port or pin is out of range.
 */
enum avr_pin_state avr_io_sample(struct avr *avr, uint8_t port, uint8_t pin);

/*
 * Similar to avr_io_sample, but treats PIN_FLOATING as PIN_LOW and packs all
 * pin data into a single byte.
 */
uint8_t avr_io_sample_port(struct avr *avr, uint8_t port);

/*
 * Simulate connecting the given pin to a voltage source, or disconnecting it.
 * The actual pin value also depends on the simulated microcontroller's inner
 * state.
 */
void avr_io_connect(struct avr *avr, uint8_t port, uint8_t pin, enum avr_pin_state value);

#endif
