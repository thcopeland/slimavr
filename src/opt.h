#ifndef CHECKS
#define CHECKS 0
#endif

#define LOG(priority, ...)          \
    do {                            \
        if (CHECKS >= priority) {   \
            printf(__VA_ARGS__);    \
        }                           \
    } while (0)
