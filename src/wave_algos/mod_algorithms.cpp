#include "mod_algorithms.h"

uint16_t sum(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return (main.val + aux.val) >> 1;
}

uint16_t difference(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME if sum is here difference isn't really necessary
    return (main.val - aux.val + max_y) >> 1;
}

uint16_t exclusive_or(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t top = main.val & 0xE000;
    uint16_t bottom = (main.val & 0x1FFF) ^ (aux.val & 0x1FFF);
    return top | bottom;
}

uint16_t invert(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME only useful for LFO and ENV
    return max_y - main.val;
}

uint16_t analog_pulse_pm(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME square wave lookup table isn't that interesting
    return analog_pulse_table[main.val >> 5];
}

uint16_t double_freq(Waveformer& main, Waveformer& aux, Modulator& mod) {
    mod.core.acc += main.core.pha << 1;
    mod.core.s_acc = mod.core.acc >> 21;
    return waveform_generator(mod.core.s_acc, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t half_freq(Waveformer& main, Waveformer& aux, Modulator& mod) {
    mod.core.acc += main.core.pha >> 1;
    mod.core.s_acc = mod.core.acc >> 21;
    return waveform_generator(mod.core.s_acc, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t rectify(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME maybe apply a litte saturation to this as well? Could be interesting
    uint32_t val = (main.val < half_y)? max_y - main.val : main.val;
    return static_cast<uint16_t>((val << 1) - max_y);
}

uint16_t crosscrush(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t crush_amt = static_cast<uint16_t>((static_cast<uint32_t>(aux.val) * (CRUSH_MAX - CRUSH_MIN)) >> 16) + CRUSH_MIN; // should go from CRUSH_MIN to CRUSH_MAX
    return (main.val >> crush_amt) << crush_amt;
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
    return waveform_generator((main.core.s_acc >> SRR_AMT) << SRR_AMT, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t sine_pm(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME this should do more, not be just a sine wave when the waveformer generates a saw
    return sine_table[main.val >> 5];
}

uint16_t ratio_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t ratio_val = aux.val >> bit_diff;
    return waveform_generator(main.core.s_acc, main.shp, ratio_val, calc_upslope(ratio_val), calc_downslope(ratio_val));
}

uint16_t shape_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    return waveform_generator(main.core.s_acc, aux.val >> bit_diff, main.rat, main.uslp, main.dslp);
}

uint16_t gate(Waveformer& main, Waveformer& aux, Modulator& mod) {
    if (aux.val <= half_y) return 0;
    return main.val;
}

uint16_t amplitude_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME this should account for ENV vs LFO / VCO, respond differently
    return static_cast<uint16_t>((((static_cast<int64_t>(main.val) - half_y) * (static_cast<int64_t>(aux.val) - half_y)) >> 15) + half_y);
}

uint16_t frequency_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME envelope set to off still creates offset
    mod.core.acc += main.core.pha + ((static_cast<int32_t>(aux.val) - half_y) << 9);
    mod.core.s_acc = mod.core.acc >> acc_downshift;
    return waveform_generator(mod.core.s_acc, main.shp, main.rat, main.uslp, main.dslp);
}

uint16_t ring_modulate(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME ring mod and AM? might bump a little
    return static_cast<uint16_t>((main.val * static_cast<uint64_t>(aux.val)) >> 16);
}

uint16_t three_voice_chorus(Waveformer& main, Waveformer& aux, Modulator& mod) {
    // FIXME not currently working
    uint16_t offset = (main.core.pha / 6) >> acc_downshift;
    uint32_t voice_1 = main.val;
    uint32_t voice_2 = waveform_generator(main.core.s_acc - offset, main.shp, main.rat, main.uslp, main.dslp);
    uint32_t voice_3 = waveform_generator(main.core.s_acc + offset, main.shp, main.rat, main.uslp, main.dslp);
    return static_cast<uint16_t>((voice_1 + voice_2 + voice_3) / 3);
}

uint16_t bitwise_and(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t top = main.val & 0xE000;
    uint16_t bottom = (main.val & 0x1FFF) & (aux.val & 0x1FFF);
    return top | bottom;
}

uint16_t sample_and_hold(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t output_val = 0;
    if (mod.param_b < trig_threshold && main.val >= trig_threshold) {  // rising edge
        output_val = aux.val;
        mod.param_a = aux.val;
    } else {
        output_val = mod.param_a;
    }
    mod.param_b = main.val;
    return output_val;
}

uint16_t bitwise_or(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint16_t top = main.val & 0xE000;
    uint16_t bottom = (main.val & 0x1FFF) | (aux.val & 0x1FFF);
    return top | bottom;
}

uint16_t rungle(Waveformer& main, Waveformer& aux, Modulator& mod) {
    if (mod.param_b < trig_threshold && main.val >= trig_threshold) {  // rising edge
        mod.param_a <<= 1; // shift register
        mod.param_a |= (aux.val > trig_threshold) ^ ((mod.param_a >> 8) & 0x0001);  // rungle!
        mod.param_a &= 0x00FF;  // shift register
    }
    mod.param_b = main.val;
    return (mod.param_a & 0x00E0) << 8;
}


uint16_t binary_and(Waveformer& main, Waveformer& aux, Modulator& mod) {
    if (main.val > trig_threshold && aux.val > trig_threshold) return max_y - 1;
    else return 0;
}

uint16_t binary_or(Waveformer& main, Waveformer& aux, Modulator& mod) {
    if (main.val > trig_threshold || aux.val > trig_threshold) return max_y - 1;
    else return 0;
}

uint16_t phase_offset(Waveformer& main, Waveformer& aux, Modulator& mod){
    uint16_t new_phase = ((2048 + main.core.s_acc - main.rat) % 2048);
    if (main.mode == ENV) {
        if (main.core.s_acc >= main.rat && mod.param_b < main.rat && main.running) mod.running = true;
        else if (mod.param_a > new_phase && mod.running) mod.running = false;
    } else mod.running = true;

    mod.param_a = new_phase;
    mod.param_b = main.core.s_acc;
    if (mod.running) return waveform_generator(new_phase, main.shp, main.rat, main.uslp, main.dslp);
    else return 0;
}

uint16_t maximum(Waveformer& main, Waveformer& aux, Modulator& mod){ 
    if (main.val > aux.val) return main.val;
    else return aux.val;
}

#define NUM_HARM_INTERVALS 8
uint16_t harmonic_fm_intervals[NUM_HARM_INTERVALS][2] = {
    {1, 8},     // octave down
    {1, 4},     // unity
    {1, 2},     // minor third
    {1, 1},     // major third
    {2, 1},     // perfect fourth
    {4, 1},     // perfect fifth
    {8, 1},     // minor seventh
    {16, 1},     // octave
};

uint16_t harmonic_fm(Waveformer& main, Waveformer& aux, Modulator& mod) {
    uint32_t interval_idx = static_cast<uint32_t>(aux.val) * NUM_HARM_INTERVALS >> 16;
    uint16_t mult_amt = harmonic_fm_intervals[interval_idx][0];
    uint16_t div_amt = harmonic_fm_intervals[interval_idx][1];
    uint32_t new_pha = (main.core.pha * mult_amt) / div_amt;
    mod.core.acc += new_pha;
    mod.core.s_acc = mod.core.acc >> acc_downshift;
    return waveform_generator(mod.core.s_acc, main.shp, main.rat, main.uslp, main.dslp);
}