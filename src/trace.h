#ifndef SLIMAVR_TRACE_H
#define SLIMAVR_TRACE_H

#ifndef AVR_DEBUG_HISTORY_SIZE
#define AVR_DEBUG_HISTORY_SIZE 32
#endif

struct avr_trace_inst {
    uint32_t addr;
    uint16_t inst;
    uint16_t inst2;
};

struct avr_tracedata {
    struct avr_trace_inst *history;
    int history_start;
    int history_end;
};

struct avr_tracedata *avr_trace_new(void);
void avr_trace_free(struct avr_tracedata *trace);
void avr_trace_reset(struct avr_tracedata *trace);
void avr_trace_enq(struct avr_tracedata *trace, uint32_t addr, uint16_t inst, uint16_t inst2);
int avr_trace_deq(struct avr_tracedata *trace, struct avr_trace_inst *inst);

#endif
