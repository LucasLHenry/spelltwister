#include <Arduino.h>

#ifndef ADC_FILTER_H
#define ADC_FILTER_H

#define ABS_SUB(a, b) (a > b)? a - b : b - a

class ADC_Filter {
    uint32_t filtered_value;
    uint16_t smooth_amt;
    uint64_t low_margin, high_margin;
    uint16_t upsample_amt;
    uint16_t smth_lo, smth_mid, smth_hi;
    public:
        ADC_Filter();//uint16_t low, uint16_t high);
        uint16_t get_next(uint64_t input);
};

#endif