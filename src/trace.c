#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "opt.h"
#include "trace.h"
#include "decode.h"
#include "avr.h"

struct avr_tracedata *avr_trace_new(void) {
    struct avr_tracedata *trace = malloc(sizeof(*trace));
    if (trace) {
        trace->history = malloc(sizeof(*trace->history)*AVR_DEBUG_HISTORY_SIZE);
        trace->history_start = 0;
        trace->history_end = 0;

        if (trace->history == NULL) {
            trace = NULL;
        }
    }
    return trace;
}

void avr_trace_free(struct avr_tracedata *trace) {
    if (trace) {
        free(trace->history);
        free(trace);
    }
}

void avr_trace_enq(struct avr_tracedata *trace, uint32_t addr, uint16_t inst, uint16_t inst2) {
    trace->history[trace->history_end].addr = addr;
    trace->history[trace->history_end].inst = inst;
    trace->history[trace->history_end].inst2 = inst2;
    trace->history_end = (trace->history_end+1) % AVR_DEBUG_HISTORY_SIZE;
    if (trace->history_start == trace->history_end) {
        trace->history_start = (trace->history_start+1) % AVR_DEBUG_HISTORY_SIZE;
    }
}

int avr_trace_deq(struct avr_tracedata *trace, struct avr_trace_inst *inst) {
    if (trace->history_start == trace->history_end) {
        return 0;
    } else if (trace->history_start < trace->history_end) {
        *inst = trace->history[trace->history_start];
        trace->history_start++;
        return 1;
    } else {
        *inst = trace->history[trace->history_start];
        trace->history_start = (trace->history_start+1) % AVR_DEBUG_HISTORY_SIZE;
        return 1;
    }
}

int avr_dump(struct avr *avr, const char *fname) {
    char inst_string[32];
    FILE *f = stdout;
    if (fname != NULL) {
        f = fopen(fname, "w");
        if (f == NULL) {
            return 0;
        }
    }

    if (avr->status == MCU_STATUS_CRASHED) {
        fprintf(f, "AVR core crashed with ");
        switch(avr->error) {
            case AVR_INVALID_INSTRUCTION:
                fprintf(f, "AVR_INVALID_INSTRUCTION");
                break;
            case AVR_UNSUPPORTED_INSTRUCTION:
                fprintf(f, "AVR_UNSUPPORTED_INSTRUCTION");
                break;
            case AVR_INVALID_RAM_ADDRESS:
                fprintf(f, "AVR_INVALID_RAM_ADDRESS");
                break;
            case AVR_INVALID_ROM_ADDRESS:
                fprintf(f, "AVR_INVALID_ROM_ADDRESS");
                break;
            case AVR_INVALID_STACK_ACCESS:
                fprintf(f, "AVR_INVALID_STACK_ACCESS");
                break;
            default:
                fprintf(f, "UNKNOWN");
                break;
        }
        fprintf(f, " (%d)\n", avr->error);
    } else {
        fprintf(f, "AVR core ok\n");
    }
    fprintf(f, "PC: 0x%06x\n", avr->pc);
    uint16_t inst = (avr->rom[avr->pc+1] << 8) | avr->rom[avr->pc],
             inst2 = (avr->rom[avr->pc+3] << 8) | avr->rom[avr->pc+2];
    avr_decode(inst_string, sizeof(inst_string), inst, inst2);
    fprintf(f, "Instruction: %s\n", inst_string);
    fprintf(f, "Registers:\n");
    for (int i = 0; i < 8; i += 1) {
        fprintf(f, " r%-2d = %3d", i, avr->reg[i]);
        if (avr->model.regsize > 8) {
            fprintf(f, "        r%-2d = %3d", i+8, avr->reg[i + 8]);

            if (avr->model.regsize > 16) {
                fprintf(f, "        r%-2d = %3d", i+16, avr->reg[i + 16]);
                fprintf(f, "        r%-2d = %3d", i+24, avr->reg[i + 24]);
            }
        }
        fprintf(f, "\n");
    }
#ifdef SLIMAVR_DEBUG_HISTORY
    fprintf(f, "\nRecently executed instructions:\n");
    struct avr_trace_inst traced;
    while (avr_trace_deq(avr->trace, &traced)) {
        avr_decode(inst_string, sizeof(inst_string), traced.inst, traced.inst2);
        fprintf(f, "0x%06x:\t%s\n", traced.addr, inst_string);
    }
#else
    fprintf(f, "\nCompile slimavr with DEBUG=1 or higher for recent instructions\n");
#endif
    return 1;
}
