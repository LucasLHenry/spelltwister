#include <Arduino.h>

#ifndef ADC_FILTER_H
#define ADC_FILTER_H

#define ABS_SUB(a, b) (a > b)? a - b : b - a

#define ARR_LEN_BITS 8
constexpr uint16_t arr_len = 1 << ARR_LEN_BITS;

enum FilterMode {
    IIR,
    AVG,
    BYPASS
};

class ADC_Filter {
    bool _debug;
    uint32_t filtered_value;
    uint16_t smooth_amt;
    uint64_t low_margin, high_margin;
    uint16_t upsample_amt;
    uint16_t smth_lo, smth_mid, smth_hi;
    uint16_t arr[arr_len];
    uint64_t avg;
    uint16_t idx, max_idx;
    FilterMode mode;
    public:
        ADC_Filter(uint16_t low, uint16_t high, bool debug);
        uint16_t get_next(uint64_t input);
};

#endif