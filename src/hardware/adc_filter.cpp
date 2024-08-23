#include "adc_filter.h"

ADC_Filter::ADC_Filter(){//uint16_t low, uint16_t high) {
        upsample_amt = 8;
        low_margin = 45 << upsample_amt;
        high_margin = 200 << upsample_amt;
        smth_lo = 0;
        smth_mid = 1;
        smth_hi = 4;
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
    if ((diff >> (upsample_amt + smooth_amt)) != 0) Serial.println(diff >> (upsample_amt + smooth_amt));
    filtered_value += (upsampled_input - filtered_value) >> smooth_amt;
    return static_cast<uint16_t>(filtered_value >> upsample_amt);
}