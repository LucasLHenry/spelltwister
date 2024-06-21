#include "waveformer.h"

// from the mux wiring, different for the two sides
uint16_t A_mux_sigs[] = {R_CV_A, R_POT_A, S_CV_A, S_POT_A, M_CV_A, SW_1_A, SW_2_A, EXP_CV_A};
uint16_t B_mux_sigs[] = {R_CV_B, R_POT_B, S_CV_B, S_POT_B, M_CV_B, SW_1_B, SW_2_B, EXP_CV_B};

Waveformer::Waveformer(bool is_A, int mux_pin, int time_pin):
    is_a {is_A},
    mux(admux::Pin(mux_pin, INPUT, admux::PinType::Analog), admux::Pinset(MUX_S0, MUX_S1, MUX_S2)),
    lin_time_pin(time_pin),
    rat_read(0, true, 0.001),
    shp_read(0, true),
    algo_read(0, true),
    time_read(0, true, 0.1)
{
    if (is_a) {
        mux_sigs = A_mux_sigs;
        configs = {264, 322, 570, 567, 570, 570};
    } else {
        mux_sigs = B_mux_sigs;
        configs = {264, 322, 570, 570, 570, 574};
    }
}

void Waveformer::init() {
    pha = 50 * HZPHASOR;
    rat = 1023;
    shp = 511;
    uslp = calc_upslope(1023);
    dslp = calc_downslope(1023);
    rat_read.setAnalogResolution(1 << BITS_ADC);
    rat_read.enableEdgeSnap();
    shp_read.setAnalogResolution(1 << BITS_ADC);
    shp_read.enableEdgeSnap();
    algo_read.setAnalogResolution(1 << BITS_ADC);
    time_read.setAnalogResolution(1 << BITS_ADC);
}

void Waveformer::update() {
    acc += pha;
    s_acc = acc >> 21;  // acc is 32b, 32-21 = 11b → 0-2047 range
}

void Waveformer::generate() {
    val = waveform_generator(s_acc, shp, rat, uslp, dslp);
}

void Waveformer::read() {
    uint16_t rat_buf = get_ratio();
    if (rat_buf != rat) {
        rat = rat_buf;
        uslp = calc_upslope(rat);
        dslp = calc_downslope(rat);
    }

    shp = get_shape();
}

uint16_t Waveformer::get_shape() {
    uint16_t raw_shape_pot = mux.read(mux_sigs[S_PT_IDX]);
    uint16_t raw_shape_cv = mux.read(mux_sigs[S_CV_IDX]);
    shp_read.update(max_adc - raw_shape_pot);
    return shp_read.getValue() >> 1;
    // minus on shape pot because it's wired the other way around
    // shp_read.update(CLIP(max_adc - raw_shape_pot + raw_shape_cv - configs.shape_offset, 0, max_adc));
    // return shp_read.getValue() >> 1;  // we use 11b values internally, so shift it down 1
    // return CLIP((max_adc - raw_shape_pot + raw_shape_cv - configs.shape_offset) >> 1, 0, max_adc) >> 1;
}

uint16_t Waveformer::get_ratio() {
    uint16_t raw_ratio_pot = mux.read(mux_sigs[R_PT_IDX]);
    uint16_t raw_ratio_cv = mux.read(mux_sigs[R_CV_IDX]);
    rat_read.update(max_adc - raw_ratio_pot);
    return rat_read.getValue() >> 1;
    // minus on ratio pot because it's wired the other way around
    // rat_read.update(CLIP(max_adc - raw_ratio_pot + raw_ratio_cv - configs.ratio_offset, 0, max_adc));
    // return rat_read.getValue() >> 1; // we use 11b values internally, so shift it down 1
}