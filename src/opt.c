#include <stdint.h>
#include <assert.h>

void check_compatibility(void) {
    volatile uint8_t u = 0xff;
    volatile int8_t s = 0xff;
    // ensure that we have arithmetic right shifts for signed numbers
    // this is ubiquitous but technically undefined behavior
    assert((uint8_t) (u >> 1) == 0x7f);
    assert((uint8_t) (s >> 1) == 0xff);
}
