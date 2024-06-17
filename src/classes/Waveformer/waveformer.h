#include <Arduino.h>
#include <Mux.h>
#include <ResponsiveAnalogRead.h>

#include "../../wave_algos/generator.h"

#ifndef WAVEFORMER_H
#define WAVEFORMER_H

class Waveformer {
    int lin_time_pin, mux_pin;
    ResponsiveAnalogRead rat_read, shp_read, time_read, algo_read;
    
    bool is_a;
    uint16_t mux_sigs;
    admux::Mux mux;
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