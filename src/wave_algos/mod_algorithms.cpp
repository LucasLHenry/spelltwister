#include "mod_algorithms.h"

uint16_t sum(Waveformer& main, Waveformer& aux, Modulator& m) {
    return (main.val + aux.val) >> 1;
}

uint16_t difference(Waveformer& main, Waveformer& aux, Modulator& m) {
    return (main.val - aux.val + max_y) >> 1;
}

uint16_t exculsive_or(Waveformer& main, Waveformer& aux, Modulator& m) {
    uint16_t top = main.val & 0xE000;
    uint16_t bottom = (main.val & 0x1FFF) ^ (aux.val & 0x1FFF);
    return top | bottom;
}

uint16_t invert(Waveformer& main, Waveformer& aux, Modulator& m) {
    return max_y - main.val;
}

uint16_t analog_pulse_pm(Waveformer& main, Waveformer& aux, Modulator& m) {
    return pgm_read_word_near(analog_pulse_table + (main.val >> 5));
}

uint16_t double_freq(Waveformer& main, Waveformer& aux, Modulator& m) {
    return waveform_generator((main.acc >> 20) % max_x, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t half_freq(Waveformer& main, Waveformer& aux, Modulator& m) {
    return waveform_generator(main.acc >> 23, main.shp, main.rat, main.uslp, main.dslp);
    return 0;
}

uint16_t rectify(Waveformer& main, Waveformer& aux, Modulator& m) {
    uint32_t val = (main.val < half_y)? max_y - main.val : main.val;
    return static_cast<uint16_t>((val << 1) - max_y);
}

uint16_t bitcrush(Waveformer& main, Waveformer& aux, Modulator& m) {
    return (main.val >> CRUSH_AMT) << CRUSH_AMT;
}


// DEEMED NOT VERY INTERESTING, SO WON'T GET INCLUDED IN FINAL RELEASE.
// also, `random` is a huge performance hit.
uint16_t noisify(Waveformer& main, Waveformer& aux, Modulator& m) {
    // #define RAND_AMT 4096
    // constexpr int16_t half_rand = RAND_AMT / 2;
    // return CLIP((int16_t)main.val + random(RAND_AMT) - half_rand, 0, max_y);
    return main.val;
}

uint16_t sample_rate_reduce(Waveformer& main, Waveformer& aux, Modulator& m) {
    #define SRR_AMT 1
    return waveform_generator((main.s_acc >> SRR_AMT) << SRR_AMT, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t wavefold(Waveformer& main, Waveformer& aux, Modulator& m) {
    return 0; //pgm_read_word_near(sine_fold + (main.val >> 6));
}

uint16_t ratio_mod(Waveformer& main, Waveformer& aux, Modulator& m) {
    return waveform_generator(main.s_acc, main.shp, aux.val >> 6, main.uslp, main.dslp);
}

uint16_t shape_mod(Waveformer& main, Waveformer& aux, Modulator& m) {
    uint16_t shape_val = aux.val >> 6;
    return waveform_generator(main.s_acc, shape_val, main.rat, calc_upslope(shape_val), calc_downslope(shape_val));
}

uint16_t gate(Waveformer& main, Waveformer& aux, Modulator& m) {
    if (aux.val <= half_y) return 0;
    return main.val;
}

uint16_t amplitude_mod(Waveformer& main, Waveformer& aux, Modulator& m) {
    return static_cast<uint16_t>((((static_cast<int64_t>(main.val) - half_y) * (static_cast<int64_t>(aux.val) - half_y)) >> 15) + half_y);
}

uint16_t frequency_mod(Waveformer& main, Waveformer& aux, Modulator& m) {
    return 0;
}

uint16_t ring_modulate(Waveformer& main, Waveformer& aux, Modulator& m) {
    return static_cast<uint16_t>((main.val * static_cast<uint64_t>(aux.val)) >> 16);
}