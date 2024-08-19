#include "adc_filter.h"

ADC_Filter::ADC_Filter():
    filtered_value(0),
    smooth_amt(5) {}


uint16_t ADC_Filter::get_next(uint16_t input) {
    uint16_t diff = ABS_SUB(input, filtered_value);
    // these numbers could be determined algorithmically, and can be optimized
    if (diff < 70) smooth_amt = 6;
    if (70 <= diff < 200) smooth_amt = 2;
    else smooth_amt = 0;
    filtered_value += (input - filtered_value) >> smooth_amt;
    return filtered_value;
}