#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif
