#include "adc_filter.h"

ADC_Filter::ADC_Filter():
    filtered_value(0),
    smooth_amt(6),
    upsample_amt(0) {
        low_margin = 35 << upsample_amt;
        high_margin = 200 << upsample_amt;
    }


uint16_t ADC_Filter::get_next(uint16_t input) {
    uint64_t upsampled_input = input << upsample_amt;
    // int64_t diff = input - static_cast<int32_t>(filtered_value);
    // if (diff < 0) diff *= -1;
    // these numbers could be determined algorithmically, and can be optimized
    // if (diff < low_margin) smooth_amt = 6;
    // if (low_margin <= diff < high_margin) smooth_amt = 2;
    // else smooth_amt = 0;
    filtered_value += (upsampled_input - filtered_value) >> smooth_amt;
    return static_cast<uint16_t>(filtered_value >> upsample_amt);
}