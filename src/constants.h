#ifndef CONSTANTS_H
#define CONSTANTS_H

#define HZPHASOR 98113
#define BITS_X 11
#define BITS_Y 16
constexpr uint16_t max_x = (1 << BITS_X) - 1;
constexpr uint16_t max_y = (1 << BITS_Y) - 1;
constexpr uint16_t half_x = (1 << (BITS_X - 1)) - 1;
constexpr uint16_t half_y = (1 << (BITS_Y - 1)) - 1;
constexpr uint16_t bit_diff = BITS_Y - BITS_X;

#define BITS_ADC 12
constexpr uint16_t max_adc = (1 << BITS_ADC) - 1;
#endif