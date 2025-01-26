#include <stdint.h>
#include "../classes/Waveformer/waveformer.h"
#include "../classes/Modulator/modulator.h"
#include "generator.h"
#include "../constants.hpp"
#include "../tables/transfer_functions/sine.h"
#include "../tables/transfer_functions/analog_pulse.h"

#ifndef MODULATION_ALGORITHMS_H
#define MODULATION_ALGORITHMS_H

uint16_t sum                 (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t difference          (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t exclusive_or        (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t invert              (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t double_freq         (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t half_freq           (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t rectify             (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t crosscrush          (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t noisify             (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t sample_rate_reduce  (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t sine_pm             (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t ratio_mod           (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t shape_mod           (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t gate                (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t amplitude_mod       (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t frequency_mod       (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t ring_modulate       (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t analog_pulse_pm     (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t three_voice_chorus  (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t bitwise_and         (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t sample_and_hold     (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t bitwise_or          (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t rungle              (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t binary_and          (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t binary_or           (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t phase_offset        (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t maximum             (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t harmonic_fm         (Waveformer& main, Waveformer& aux, Modulator& mod);
#endif