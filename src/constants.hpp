#include <Arduino.h>

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

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
static_assert(max_adc == 4095);

#define PWM_FREQ_kHz 44.1
#define BIT_DEPTH 11
#define CLOCK_FREQ 125000000
#define LOOPS_PER_SEC 1018

#define FM_ATTENUATION 8
#define FM_AMT_TO_PHA 9811300

#define TRIG_LENGTH_MS 0.25
#define TRIG_LED_LENGTH_MS 75

// modulation algorithm settings
#define CRUSH_AMT 12
#define FM_ALGO_AMT 10
#define SRR_AMT 1

const uint32_t min_pha = 2834160;
const uint32_t max_pha = 725544960;
const uint32_t min_slow_pha = 9447;
const uint32_t max_slow_pha = 3778880;
constexpr int64_t env_mapping_scale = (min_slow_pha - static_cast<int64_t>(max_slow_pha)) / max_adc;
constexpr uint64_t env_mapping_offset = max_slow_pha;
static_assert(env_mapping_scale == -920);

#endif