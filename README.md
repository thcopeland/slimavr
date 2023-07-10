## Slimavr

Slimavr is a slim, fast emulator for a subset of 8-bit AVR microcontrollers. The name follows in the grand tradition established by [simulavr](https://www.nongnu.org/simulavr/) and [simavr](https://github.com/buserror/simavr).

Unlike these older projects, slimavr is designed to run at or faster than the standard AVR 16 MHz. On my 2.6 GHz laptop it can achieve around 38 MHz on [this benchmark](tests/asm/benchmark.S). Performance strongly depends on the program, however. Programs using several timers tend to be slower. Programs that use prescaled timers (or none at all) can be much faster.

However, unless you need this sort of performance, you should use simavr. It is far more complete and stable. In fact, even if you do need slimavr's speed, I'd strongly recommend testing frequently with simavr (or better still, a physical MCU) to ensure that things work properly.

## Supported devices

Currently, the following devices are supported:

- ATmega 1280/2560

which is not very many. Neither is this support complete by any means. If your project needs some other MCU, please create an Issue or PR.

## Missing features

Slimavr is far from complete. At this point, it lacks

 - Fuse emulation
 - External interrupts
 - External timers
 - External memory
 - Any external communication beyond simple IO (no USART, I2C, SPI, etc)
 - ICRn timer modes
 - Sleep modes (IDLE mode partially supported)
 - Watchdog timer
 - ADC simulation
 - `xch`, `las`, `lac`, `lat`, `break`, and `wdr` instructions
 - and more!

The features that are implemented, however, should be fairly accurate and complete, however.

## Installation

Download the latest version from the [releases page](https://github.com/thcopeland/slimavr/releases). Alternatively, clone the repository and run `./releases.sh` to create your own release.

## Usage

Slimavr is intended to be placed into your project source and compiled with it. Therefore, the included Makefile will produce a static library, `libslimavr.a`, which you should link against. Where necessary, you should `#include slimavr-X-X-X/slimavr.h`.

To create an emulated device, use
```c
struct avr *avr_new(struct avr_model model);
```
where `model` is some supported model, such as `AVR_MODEL_ATMEGA1280`. If memory allocations fail, `avr_init` will return `NULL`.

To loaded a file in Intel HEX format (the only supported format), use
```c
int avr_load_ihex(struct avr *avr, char *fname);
```
If loading the file succeeds, `avr_load_ihex` will return `0`. Otherwise, it will print the failure reason and return `AVR_EFILE`, `AVR_EFORMAT`, or `AVR_ECHECKSUM`.

To advance the device by a single cycle, use
```c
void avr_step(struct avr *avr);
```
Note that, unlike simavr, this does not necessary execute a full instruction.

To restart the device, use
```c
void avr_reset(struct avr *avr);
```
A restarted device is identical to a newly-created device, except that the pin connections (as configured with `avr_io_write`) are not changed.

In order to interact with and examine the emulated device, you can access its memory through the `mem`, `rom`, `reg`, `ram`, or `eep` fields. These segments are arranged in memory so that they can be accessed through `mem` in the usual address space (R0-R31, register file, SRAM).

You can access model information through the `model` field, though you should probably avoid doing so, as this might change in future versions.

Periodically, you should check that the device has not crashed by examining the `status` field.

```c
struct avr {
    struct avr_model model;     // processor model
    enum avr_error error;       // current error, if any
    enum avr_status status;     // processor state
    int8_t progress;            // cycles remaining for multi-cycle instructions
    uint32_t pc;                // program counter
    uint64_t clock;             // number of cycles
    uint64_t insts;             // number of instructions

    // memory
    uint8_t *mem;

    // memory segments
    uint8_t *rom;               // program memory
    uint8_t *reg;               // registers
    uint8_t *ram;               // sram
    uint8_t *eep;               // eeprom

    // + various internal state
};
```

If you want to communicate with the emulated core, two helper functions are provided to ensure that the proper side effects are triggered. `avr_io_read` simulates sampling the voltage at the given pin, and `avr_io_write` simulates connecting the given pin to a voltage source.
```c
enum avr_pin_state avr_io_read(const struct avr *avr, char port, uint8_t pin);
void avr_io_write(struct avr *avr, char port, uint8_t pin, enum avr_pin_state value);
```
Alternatively, if you need more precise control, you can access IO registers directly through `avr->mem`. This is simple and fast but doesn't trigger interrupts or account for pullup resistor configurations.

If the emulated MCU crashes, you can use `avr_dump` to inspect the current state and recently executed instructions.
```c
int avr_dump(struct avr *avr, const char *fname);
```

When you've finished with the device, use
```c
void avr_free(struct avr *avr);
```
to release its memory.
