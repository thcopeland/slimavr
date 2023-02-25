#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <simavr/sim_avr.h>
#include <simavr/avr_ioport.h>
#include <simavr/sim_hex.h>

#include "src/slimavr.h"

struct avr_t *simavr_init(char *fname) {
    struct avr_t *avr = avr_make_mcu_by_name("atmega1280");
    avr_init(avr);
    avr->frequency = 16000000;
    ihex_chunk_p chunks;
    int chunk_count = read_ihex_chunks(fname, &chunks);
    avr->pc = 0;
    for (int i = 0; i < chunk_count; i++) {
        ihex_chunk_t chunk = chunks[i];
        memcpy(avr->flash + chunk.baseaddr, chunk.data, chunk.size);
    }
    free_ihex_chunks(chunks);
    return avr;
}

struct avr *slimavr_init(char *fname) {
    struct avr *avr = avr_new(AVR_MODEL_ATMEGA1280);
    avr_load_ihex(avr, fname);
    return avr;
}

void simavr_step(struct avr_t *avr) {
    int state = avr_run(avr);

    if (state == cpu_Crashed) {
        printf("simavr crashed\n");
    }
}

void slimavr_step(struct avr *avr) {
    avr_step(avr);

    if (avr->status == MCU_STATUS_CRASHED) {
        printf("slimavr crashed\n");
    } else {
        while (avr->status != MCU_STATUS_NORMAL) avr_step(avr);
    }
}

int pc_eq(struct avr_t *ref, struct avr *test) {
    return ref->pc == test->pc;
}

int compare(struct avr_t *ref, struct avr *test, char *err) {
    if (!pc_eq(ref, test)) {
        sprintf(err, "Program counters mismatched (expected %x, got %x)\n", ref->pc, test->pc);
        return 1;
    }

    if (ref->data[0x5d] != test->mem[0x5d] || ref->data[0x5e] != test->mem[0x5e]) {
        sprintf(err, "Stack pointer mismatch (expected %x, got %x)\n", ref->data[0x5d] | (ref->data[0x5e] << 8), test->mem[0x5d] | (test->mem[0x5e] << 8));
        return 1;
    }

    uint8_t sreg = ((ref->sreg[0] != 0) << 0) |
                   ((ref->sreg[1] != 0) << 1) |
                   ((ref->sreg[2] != 0) << 2) |
                   ((ref->sreg[3] != 0) << 3) |
                   ((ref->sreg[4] != 0) << 4) |
                   ((ref->sreg[5] != 0) << 5) |
                   ((ref->sreg[6] != 0) << 6) |
                   ((ref->sreg[7] != 0) << 7);

    if (sreg != test->mem[0x5f]) {
        sprintf(err, "SREG mismatch (expected %x, got %x)\n", sreg, test->mem[0x5f]);
        return 1;
    }

    return 0;
}

// how many steps to catch up (helps with mismatched timers)
// #define PC_FUZZ 100

void step_both(struct avr_t *ref, struct avr *test) {
    char err[128];

    uint32_t simavr_old_pc = ref->pc,
             slimavr_old_pc = test->pc,
             simavr_new_pc,
             slimavr_new_pc;

    simavr_step(ref);
    simavr_new_pc = ref->pc;

    slimavr_step(test);
    slimavr_new_pc = test->pc;

    if (ref->pc != test->pc) {
        while (ref->pc != test->pc) {
            printf("1. simavr: %x\n2. slimavr: %x\n", ref->pc, test->pc);
            char x = getchar();

            if (x == 'q') exit(1);
            else if (x == '2') slimavr_step(test);
            else if (x == '1') simavr_step(ref);
        }
    }

    // if (simavr_new_pc == simavr_old_pc && slimavr_new_pc != slimavr_old_pc) {
    //     // wait for simavr to catch up
    //     for (int i = 0; i < PC_FUZZ && !pc_eq(ref, test); i++) {
    //         simavr_step(ref);
    //     }
    // } else if (simavr_new_pc != simavr_old_pc && slimavr_new_pc == slimavr_old_pc) {
    //     // wait for slimavr to catch up
    //     for (int i = 0; i < PC_FUZZ && !pc_eq(ref, test); i++) {
    //         slimavr_step(test);
    //     }
    // }

    if (compare(ref, test, err) != 0) {
        printf("Failed! %s\n", err);
        exit(1);
    }
}

int main(int argc, char **argv) {
    struct avr_t *ref = simavr_init("tests/tough.hex");
    struct avr *test = slimavr_init("tests/tough.hex");

    while (ref->data[0x31] != 0xff) {
    // for (int i = 0; i < 1000; i++) {
        step_both(ref, test);
    }
}
