#ifndef OPT_H
#define OPT_H

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

void check_compatibility(void);

#endif
