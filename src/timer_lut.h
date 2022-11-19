#define NOP 0
#define CLR 1
#define SET 2
#define FLP 3

// Compare Output Mode (COM) lookup tables

// These determine how various COMnx and OCRnx values drive the associated output
// pins for various Waveform Generation Modes (WGMs).
//
// Each row has four values that specify whether to do nothing, clear, set, or
// flip the pin when an event occurs. For each column, these events are OCRnx
// match while upcounting, OCRnx match while downcounting, TOP match, and
// BOTTOM (which is always 0) match.
//
// The first four rows deal with the case where OCRnx != TOP and OCRnx != BOTTOM.
// The next four when OCRnx == TOP and OCRnx != BOTTOM, and the next four when
// OCRnx != TOP and OCRnx == BOTTOM. Since the minimum timer resolution is 2-bit
// (OCRnA or ICRn set to 0x0003), it is impossible for TOP == BOTTOM, and hence
// we don't need to consider OCRnx == TOP and OCRnx == BOTTOM.

static const uint8_t empty_com_table[48] = {
    NOP, NOP, NOP, NOP,    // COMnx 00
    NOP, NOP, NOP, NOP,    // COMnx 01
    NOP, NOP, NOP, NOP,    // COMnx 10
    NOP, NOP, NOP, NOP,    // COMnx 11

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == TOP
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == TOP
    NOP, NOP, NOP, NOP,    // COMnx 10 and OCRnx == TOP
    NOP, NOP, NOP, NOP,    // COMnx 11 and OCRnx == TOP

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == BOTTOM
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == BOTTOM
    NOP, NOP, NOP, NOP,    // COMnx 10 and OCRnx == BOTTOM
    NOP, NOP, NOP, NOP,    // COMnx 11 and OCRnx == BOTTOM
};

static const uint8_t non_pwm_com_table[48] = {
    NOP, NOP, NOP, NOP,    // COMnx 00
    FLP, NOP, NOP, NOP,    // COMnx 01
    CLR, NOP, NOP, NOP,    // COMnx 10
    SET, NOP, NOP, NOP,    // COMnx 11

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == TOP
    FLP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == TOP
    CLR, NOP, NOP, NOP,    // COMnx 10 and OCRnx == TOP
    SET, NOP, NOP, NOP,    // COMnx 11 and OCRnx == TOP

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == BOTTOM
    FLP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == BOTTOM
    CLR, NOP, NOP, NOP,    // COMnx 10 and OCRnx == BOTTOM
    SET, NOP, NOP, NOP,    // COMnx 11 and OCRnx == BOTTOM
};

static const uint8_t fast_pwm_com_table1[48] = {
    NOP, NOP, NOP, NOP,    // COMnx 00
    NOP, NOP, NOP, NOP,    // COMnx 01
    CLR, NOP, NOP, SET,    // COMnx 10
    SET, NOP, NOP, CLR,    // COMnx 11

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == TOP
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == TOP
    NOP, NOP, NOP, SET,    // COMnx 10 and OCRnx == TOP
    NOP, NOP, NOP, CLR,    // COMnx 11 and OCRnx == TOP

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == BOTTOM
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == BOTTOM
    NOP, NOP, SET, CLR,    // COMnx 10 and OCRnx == BOTTOM
    NOP, NOP, SET, CLR,    // COMnx 11 and OCRnx == BOTTOM
};

static const uint8_t fast_pwm_com_table2[48] = {
    NOP, NOP, NOP, NOP,    // COMnx 00
    FLP, NOP, NOP, NOP,    // COMnx 01 (only difference from fast_pwm_com_table1)
    CLR, NOP, NOP, SET,    // COMnx 10
    SET, NOP, NOP, CLR,    // COMnx 11

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == TOP
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == TOP
    NOP, NOP, NOP, SET,    // COMnx 10 and OCRnx == TOP
    NOP, NOP, NOP, CLR,    // COMnx 11 and OCRnx == TOP

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == BOTTOM
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == BOTTOM
    NOP, NOP, SET, CLR,    // COMnx 10 and OCRnx == BOTTOM
    NOP, NOP, SET, CLR,    // COMnx 11 and OCRnx == BOTTOM
};

static const uint8_t phase_freq_pwm_com_table1[48] = {
    NOP, NOP, NOP, NOP,    // COMnx 00
    NOP, NOP, NOP, NOP,    // COMnx 01
    CLR, SET, NOP, NOP,    // COMnx 10
    SET, CLR, NOP, NOP,    // COMnx 11

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == TOP
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == TOP
    NOP, NOP, NOP, SET,    // COMnx 10 and OCRnx == TOP
    NOP, NOP, NOP, CLR,    // COMnx 11 and OCRnx == TOP

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == BOTTOM
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == BOTTOM
    NOP, NOP, NOP, CLR,    // COMnx 10 and OCRnx == BOTTOM
    NOP, NOP, NOP, SET,    // COMnx 11 and OCRnx == BOTTOM
};

static const uint8_t phase_freq_pwm_com_table2[48] = {
    NOP, NOP, NOP, NOP,    // COMnx 00
    FLP, NOP, NOP, NOP,    // COMnx 01
    CLR, SET, NOP, NOP,    // COMnx 10
    SET, CLR, NOP, NOP,    // COMnx 11

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == TOP
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == TOP
    FLP, NOP, NOP, NOP,    // COMnx 10 and OCRnx == TOP
    FLP, NOP, NOP, NOP,    // COMnx 11 and OCRnx == TOP

    NOP, NOP, NOP, NOP,    // COMnx 00 and OCRnx == BOTTOM
    NOP, NOP, NOP, NOP,    // COMnx 01 and OCRnx == BOTTOM
    NOP, NOP, NOP, CLR,    // COMnx 10 and OCRnx == BOTTOM
    NOP, NOP, NOP, SET,    // COMnx 11 and OCRnx == BOTTOM
};
