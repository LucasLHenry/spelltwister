#include "adc_filter.h"

ADC_Filter::ADC_Filter(uint16_t low, uint16_t high, bool debug) {
        upsample_amt = 8;
        low_margin = low << upsample_amt;
        high_margin = high << upsample_amt;
        smth_lo = 0;
        smth_mid = 1;
        smth_hi = 3;
        filtered_value = 0;
        smooth_amt = smth_hi;
    }


uint16_t ADC_Filter::get_next(uint64_t input) {
    uint64_t upsampled_input = input << upsample_amt;
    uint64_t diff = ABS_SUB(upsampled_input, filtered_value);

    if (diff < low_margin) {
        smooth_amt = smth_hi;
    } else if (low_margin <= diff && diff < high_margin) {
        smooth_amt = smth_mid;
    } else {
        smooth_amt = smth_lo;
    }
    filtered_value += (upsampled_input - filtered_value) >> smooth_amt;
    return static_cast<uint16_t>(filtered_value >> upsample_amt);
}