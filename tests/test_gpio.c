#include <stdio.h>
#include "slimavr.h"

void test_gpio(void) {
    printf("testing gpio... ");
    struct avr *avr = avr_new(AVR_MODEL_ATMEGA1280);
    avr_io_connect(avr, 0, 0, PIN_PULLDOWN);

    if (avr_load_ihex(avr, "asm/gpio.hex") == 0) {
        enum avr_pin_state last_clk = PIN_LOW;
        uint8_t count = 0;
        uint8_t wait = 10;
        for (long i = 0; i < 500; i++) {
            avr_step(avr);

            if (avr->status == MCU_STATUS_CRASHED) {
                avr_dump(avr, NULL);
                break;
            }

            if (count < 3) {
                // receive three pulses
                enum avr_pin_state clk = avr_io_sample(avr, 0, 0);
                if (last_clk == PIN_LOW && clk == PIN_HIGH) {
                    count += 1;
                }
                last_clk = clk;
            } else if (count < 6) {
                // send three pulses
                if (wait == 0) {
                    avr_io_connect(avr, 0, 0, avr_io_sample(avr, 0, 0) == PIN_HIGH ? PIN_LOW : PIN_HIGH);
                    wait = 10;
                    count += avr_io_sample(avr, 0, 0) == PIN_LOW;
                } else {
                    wait--;
                }
            }
        }

        if (avr->ram[0x42] != 0) {
            printf("fail (%d)\n", avr->ram[0x42]);
        } else {
            printf("pass\n");
        }
    }
    avr_free(avr);
}

int main(int argc, char **argv) {
    test_gpio();
    return 0;
}
