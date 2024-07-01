#include <Arduino.h>
#include "../Waveformer/waveformer.h"
#include "../LedRing/led_ring.h"
#include "../../wave_algos/generator.h"
#include "../../wave_algos/mod_algorithms.h"

#ifndef MODULATOR_H
#define MODULATOR_H

class Modulator {
    Waveformer& a;
    Waveformer& b;
    LedRing& algo_ring;
    algo_f_ptr* ring_algos;
    public:
        Modulator(Waveformer& A, Waveformer& B, LedRing& ring, algo_f_ptr* algo_arr);
        void generate_a();
        void generate_b();
        uint16_t a_val, b_val;
};

#endif