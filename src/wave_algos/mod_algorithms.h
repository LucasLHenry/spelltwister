#include <Arduino.h>
#include "../classes/Waveformer/waveformer.h"
#include "../classes/Modulator/modulator.h"
#include "generator.h"
#include "../constants.hpp"
#include "simple_mod_algos.h"

#ifndef MODULATION_ALGORITHMS_H
#define MODULATION_ALGORITHMS_H

uint16_t sum                 (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t difference          (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t exclusive_or        (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t invert              (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t double_freq         (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t half_freq           (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t rectify             (Waveformer& main, Waveformer& aux, Modulator& mod);
uint16_t bitcrush            (Waveformer& main, Waveformer& aux, Modulator& mod);
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

#endif