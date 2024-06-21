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
    pha = 50 * HZPHASOR;
    rat = 511;
    shp = 511;
    uslp = calc_upslope(511);
    dslp = calc_downslope(511);
}

void Waveformer::update() {
    acc += pha;
    s_acc = acc >> 22;  // acc is 32b, 32-22 = 10b → 0-1023 range
}

void Waveformer::generate() {
    val = waveform_generator(s_acc, shp, rat, uslp, dslp);
}