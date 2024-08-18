#include "adc_filter.h"

ADC_Filter::ADC_Filter():
    filtered_value(0),
    smooth_amt(5) {}


uint16_t ADC_Filter::get_next(uint16_t input) {
    uint16_t diff = ABS_SUB(input, filtered_value);
    smooth_amt = (diff > 500)? 2 : 6;
    filtered_value += (input - filtered_value) >> smooth_amt;
    return filtered_value;
}