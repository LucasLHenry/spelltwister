#include "mod_algorithms.h"

uint16_t sum(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return (main.val + aux.val) >> 1;
}

uint16_t difference(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return (main.val - aux.val + max_y) >> 1;
}

uint16_t exculsive_or(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t top = main.val & 0xE000;
    uint16_t bottom = (main.val & 0x1FFF) ^ (aux.val & 0x1FFF);
    return top | bottom;
}

uint16_t invert(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return max_y - main.val;
}

uint16_t analog_pulse_pm(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return analog_pulse_table[main.val >> 5]; // (analog_pulse_table + (main.val >> 5));
}

uint16_t double_freq(Waveformer& main, Waveformer& aux, Modulator& mod) {
    mod.acc += main.pha << 1;
    mod.s_acc = mod.acc >> 21;
    return waveform_generator(mod.s_acc, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t half_freq(Waveformer& main, Waveformer& aux, Modulator& mod) {
    mod.acc += main.pha >> 1;
    mod.s_acc = mod.acc >> 21;
    return waveform_generator(mod.s_acc, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t rectify(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint32_t val = (main.val < half_y)? max_y - main.val : main.val;
    return static_cast<uint16_t>((val << 1) - max_y);
}

uint16_t bitcrush(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return (main.val >> CRUSH_AMT) << CRUSH_AMT;
}


// DEEMED NOT VERY INTERESTING, SO WON'T GET INCLUDED IN FINAL RELEASE.
// also, `random` is a huge performance hit.
uint16_t noisify(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // #define RAND_AMT 4096
    // constexpr int16_t half_rand = RAND_AMT / 2;
    // return CLIP((int16_t)main.val + random(RAND_AMT) - half_rand, 0, max_y);
    return main.val;
}

uint16_t sample_rate_reduce(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return waveform_generator((main.s_acc >> SRR_AMT) << SRR_AMT, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t sine_pm(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return sine_table[main.val >> 5]; // pgm_read_word_near(sine_table + (main.val >> 5));
}

uint16_t ratio_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t ratio_val = aux.val >> bit_diff;
    return waveform_generator(main.s_acc, main.shp, ratio_val, calc_upslope(ratio_val), calc_downslope(ratio_val));
}

uint16_t shape_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return waveform_generator(main.s_acc, aux.val >> bit_diff, main.rat, main.uslp, main.dslp);
}

uint16_t gate(Waveformer& main, Waveformer& aux, Modulator& mod) {
    if (aux.val <= half_y) return (main.mode == ENV)? 0 : half_y;
    return main.val;
}

uint16_t amplitude_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return static_cast<uint16_t>((((static_cast<int64_t>(main.val) - half_y) * (static_cast<int64_t>(aux.val) - half_y)) >> 15) + half_y);
}

uint16_t frequency_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    mod.acc += main.pha + ((static_cast<int32_t>(aux.val) - max_y) << FM_ALGO_AMT);
    mod.s_acc = mod.acc >> 21;
    return waveform_generator(mod.s_acc, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t ring_modulate(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return static_cast<uint16_t>((main.val * static_cast<uint64_t>(aux.val)) >> 16);
}

uint16_t three_voice_chorus(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t offset = (main.pha / 6) >> 21;
    uint32_t voice_1 = main.val;
    uint32_t voice_2 = waveform_generator(main.s_acc - offset, main.shp, main.rat, main.uslp, main.dslp);
    uint32_t voice_3 = waveform_generator(main.s_acc + offset, main.shp, main.rat, main.uslp, main.dslp);
    return static_cast<uint16_t>((voice_1 + voice_2 + voice_3) / 3);
}