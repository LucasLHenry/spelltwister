#include <Arduino.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define HZPHASOR 98113
#define BITS_X 11
#define BITS_Y 16
constexpr uint16_t max_x = (1 << BITS_X) - 1;
constexpr uint16_t max_y = (1 << BITS_Y) - 1;
constexpr uint16_t half_x = (1 << (BITS_X - 1)) - 1;
constexpr uint16_t half_y = (1 << (BITS_Y - 1)) - 1;
constexpr uint16_t bit_diff = BITS_Y - BITS_X;  // 5

#define BITS_ADC 12
constexpr uint16_t max_adc = (1 << BITS_ADC) - 1;
constexpr uint16_t half_adc = (1 << (BITS_ADC - 1)) - 1;

#define PWM_FREQ_kHz 44.1
#define BIT_DEPTH 11
#define CLOCK_FREQ 125000000

#define FM_ATTENUATION 8

#define TRIG_LENGTH_MS 0.25
#define TRIG_LED_LENGTH_MS 75

// modulation algorithm settings
#define CRUSH_AMT 12
#define FM_ALGO_AMT 10
#define SRR_AMT 1

#endif