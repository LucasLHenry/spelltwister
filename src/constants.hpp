#include <stdint.h>

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define BITS_X 11
#define BITS_Y 16
#define ACC_BITS 32
constexpr uint16_t max_x = (1 << BITS_X) - 1;
constexpr uint16_t max_y = (1 << BITS_Y) - 1;
constexpr uint64_t max_acc = (1ull << ACC_BITS) - 1;
constexpr uint16_t half_x = (1 << (BITS_X - 1)) - 1;
constexpr uint16_t half_y = (1 << (BITS_Y - 1)) - 1;
constexpr uint16_t bit_diff = BITS_Y - BITS_X;  // 5
constexpr uint16_t acc_downshift = ACC_BITS - BITS_X;

#define BITS_ADC 12
constexpr uint16_t max_adc = (1 << BITS_ADC) - 1;
constexpr uint16_t half_adc = (1 << (BITS_ADC - 1)) - 1;

#define PWM_FREQ_kHz 44.1
#define BIT_DEPTH 11
#define CLOCK_FREQ 125000000
#define LOOPS_PER_SEC 1018

#define FM_ATTENUATION 6

#define TRIG_LENGTH_MS 3
#define TRIG_LED_LENGTH_MS 75

// modulation algorithm settings
#define CRUSH_MIN 13
#define CRUSH_MAX 16
#define FM_ALGO_AMT 8
#define SRR_AMT 7
constexpr uint16_t trig_threshold = half_y / 2 * 3;

#endif