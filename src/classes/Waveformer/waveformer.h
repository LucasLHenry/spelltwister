#include <Arduino.h>

#include "../../wave_algos/generator.h"

#ifndef WAVEFORMER_H
#define WAVEFORMER_H

class Waveformer {
    int pri_out_pin, sec_out_pin;
    uint16_t rat, shp;
    uint16_t upslope, downslope;
    public:
        uint32_t acc, pha;
        uint16_t shifted_acc;
        uint16_t val;
        Waveformer(int pri_pin, int sec_pin);
        void init();
        void update();
        void generate();
};

#endif