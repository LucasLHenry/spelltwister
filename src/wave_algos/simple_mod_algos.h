#include <Arduino.h>
#include "../constants.hpp"
#include "../tables/transfer_functions/sine.h"
#include "../tables/transfer_functions/analog_pulse.h"
// modulation algorithms that only take in the current values of either waveformer,
// or just one. Called by mod_algorithms to have a uniform signature

#ifndef SIMPLE_MOD_ALGOS_H
#define SIMPLE_MOD_ALGOS_H

uint16_t _sum(uint16_t v1, uint16_t v2);
uint16_t _difference(uint16_t v1, uint16_t v2);
uint16_t _exclusive_or(uint16_t v1, uint16_t v2);
uint16_t _invert(uint16_t v1);
uint16_t _analog_pulse_pm(uint16_t v1);
uint16_t _rectify(uint16_t v1);
uint16_t _bitcrush(uint16_t v1);
uint16_t _sine_pm(uint16_t v1);
uint16_t _gate(uint16_t v1, uint16_t v2);
uint16_t _amplitude_mod(uint16_t v1, uint16_t v2);
uint16_t _ring_modulate(uint16_t v1, uint16_t v2);

#endif // SIMPLE_MOD_ALGOS_H