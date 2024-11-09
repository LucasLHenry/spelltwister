#include <stdint.h>
#include "../Waveformer/waveformer.h"
#include "../LedRing/led_ring.h"
#include "../../wave_algos/generator.h"

#ifndef MODULATOR_H
#define MODULATOR_H

class Modulator;
typedef uint16_t (*algo_f_ptr)(Waveformer& main, Waveformer& aux, Modulator& m);

typedef enum {
    OFF,
    XFADE_DOWN,
    XFADE_UP
} XFadeMode;

class Modulator {
    Waveformer& _main;
    Waveformer& _aux;
    LedRing& algo_ring;
    algo_f_ptr* ring_algos;
    bool is_a;
    uint16_t incoming_idx, _idx;
    uint16_t xfade_amt;
    uint16_t xfade_len;
    XFadeMode xfade_mode;
    public:
        Modulator(Waveformer& main, Waveformer& aux, LedRing& ring, algo_f_ptr* algo_arr);
        void generate();
        uint16_t val;
        DDS_Wrapper core;
};

#endif