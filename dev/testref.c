#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <simavr/sim_avr.h>
#include <simavr/avr_ioport.h>
#include <simavr/sim_hex.h>

int main(int argc, char **argv) {
    struct avr_t *avr = avr_make_mcu_by_name("atmega1280");
    avr_init(avr);
    avr->frequency = 16000000;
    ihex_chunk_p chunks;
    int chunk_count = read_ihex_chunks("tests/asm/eeprom.hex", &chunks);
    avr->pc = 0;
    for (int i = 0; i < chunk_count; i++) {
        ihex_chunk_t chunk = chunks[i];
        memcpy(avr->flash + chunk.baseaddr, chunk.data, chunk.size);
    }
    free_ihex_chunks(chunks);

    const long duration = 300000;

    for (long i = 0; i < duration; i++) {
        int pc = avr->pc;

        int state = avr_run(avr);

        if (state == cpu_Crashed) {
            fprintf(stderr, "CPU crashed\n");
            exit(0);
        }
    }

    // printf("r16 = %d\n", avr->data[16]);
    // printf("r20 = %d\n", avr->data[20]);

    if (avr->data[0x242] != 0) {
        printf("fail (%d)\n", avr->data[0x242]);
    } else {
        printf("pass\n");
    }

    return 0;
}
