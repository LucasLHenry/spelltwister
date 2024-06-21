#include "waveformer.h"

Waveformer::Waveformer(bool is_A, int mux_pin, int time_pin):
    is_a {is_A},
    mux(admux::Pin(mux_pin, INPUT, admux::PinType::Analog), admux::Pinset(MUX_S0, MUX_S1, MUX_S2)),
    lin_time_pin(time_pin),
    rat_read(0, true),
    shp_read(0, true),
    algo_read(0, true),
    time_read(0, true, 0.1)
{

}

void Waveformer::init() {

}

void Waveformer::update() {

}

void Waveformer::generate() {

}