#include <Arduino.h>

#ifndef ADC_FILTER_H
#define ADC_FILTER_H

#define ABS_SUB(a, b) (a > b)? a - b : b - a

class ADC_Filter {
    uint16_t filtered_value;
    uint16_t smooth_amt;
    public:
        ADC_Filter();
        uint16_t get_next(uint16_t input);
};

#endif