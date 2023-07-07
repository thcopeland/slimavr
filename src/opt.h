#ifndef SLIMAVR_OPT_H
#define SLIMAVR_OPT_H

#ifndef DEBUG
#define DEBUG 0
#endif

// Debug levels:
// 0: don't track anything unnecessary at runtime, best performance (default)
// 1: keep track of the most recently executed instructions, loses around 5 MHz
// 2: enables runtime logging, very slow

#if DEBUG >= 1
    #define SLIMAVR_DEBUG_HISTORY
#endif

#if DEBUG >= 2
    #define SLIMAVR_DEBUG_LOG
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...) ((void)0)
#endif

#endif
