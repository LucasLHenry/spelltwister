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

// uint16_t analog_ring_mod(Waveformer& main, Waveformer& aux, Modulator& mod) {
//     uint32_t carrier = main.val << 1;
//     uint32_t other = static_cast<uint16_t>(aux.val);
//     uint32_t ring = diode_sim(other + carrier) + diode_sim(other - carrier);
//     return static_cast<uint16_t>(ring);
// }

// uint32_t uabs(uint32_t x) {
//     if (x < half_y) return half_y - x;
//     return x;
// }

// uint32_t diode_sim(uint32_t val) {
//     constexpr uint32_t offset = 3 * (half_y >> 1);
//     bool is_positive = val > half_y;
//     uint32_t dead_zone = uabs(val) - offset;
//     dead_zone += uabs(dead_zone);
//     dead_zone *= (dead_zone + 1417);
//     return (is_positive)? dead_zone : half_y - dead_zone;
// }


// EMILIE GILLET WARPS ALGORITHMS FOR REFERENCE
// inline float Modulator::Diode(float x) {
//   // Approximation of diode non-linearity from:
//   // Julian Parker - "A simple Digital model of the diode-based ring-modulator."
//   // Proc. DAFx-11
//   float sign = x > 0.0f ? 1.0f : -1.0f;
//   float dead_zone = fabs(x) - 0.667f;
//   dead_zone += fabs(dead_zone);
//   dead_zone *= dead_zone;
//   return 0.04324765822726063f * dead_zone * sign;
// }

// /* static */
// template<>
// inline float Modulator::Xmod<ALGORITHM_XFADE>(
//     float x_1, float x_2, float parameter) {
//   float fade_in = Interpolate(lut_xfade_in, parameter, 256.0f);
//   float fade_out = Interpolate(lut_xfade_out, parameter, 256.0f);
//   return x_1 * fade_in + x_2 * fade_out;
// }

// /* static */
// template<>
// inline float Modulator::Xmod<ALGORITHM_FOLD>(
//     float x_1, float x_2, float parameter) {
//   float sum = 0.0f;
//   sum += x_1;
//   sum += x_2;
//   sum += x_1 * x_2 * 0.25f;
//   sum *= 0.02f + parameter;
//   const float kScale = 2048.0f / ((1.0f + 1.0f + 0.25f) * 1.02f);
//   return Interpolate(lut_bipolar_fold + 2048, sum, kScale);
// }

// /* static */
// template<>
// inline float Modulator::Xmod<ALGORITHM_ANALOG_RING_MODULATION>(
//     float modulator, float carrier, float parameter) {
//   carrier *= 2.0f;
//   float ring = Diode(modulator + carrier) + Diode(modulator - carrier);
//   ring *= (4.0f + parameter * 24.0f);
//   return SoftLimit(ring);
// }

// /* static */
// template<>
// inline float Modulator::Xmod<ALGORITHM_DIGITAL_RING_MODULATION>(
//     float x_1, float x_2, float parameter) {
//   float ring = 4.0f * x_1 * x_2 * (1.0f + parameter * 8.0f);
//   return ring / (1.0f + fabs(ring));
// }

// /* static */
// template<>
// inline float Modulator::Xmod<ALGORITHM_XOR>(
//     float x_1, float x_2, float parameter) {
//   short x_1_short = Clip16(static_cast<int32_t>(x_1 * 32768.0f));
//   short x_2_short = Clip16(static_cast<int32_t>(x_2 * 32768.0f));
//   float mod = static_cast<float>(x_1_short ^ x_2_short) / 32768.0f;
//   float sum = (x_1 + x_2) * 0.7f;
//   return sum + (mod - sum) * parameter;
// }

// /* static */
// template<>
// inline float Modulator::Xmod<ALGORITHM_COMPARATOR>(
//     float modulator, float carrier, float parameter) {
//   float x = parameter * 2.995f;
//   MAKE_INTEGRAL_FRACTIONAL(x)

//   float direct = modulator < carrier ? modulator : carrier;
//   float window = fabs(modulator) > fabs(carrier) ? modulator : carrier;
//   float window_2 = fabs(modulator) > fabs(carrier)
//       ? fabs(modulator)
//       : -fabs(carrier);
//   float threshold = carrier > 0.05f ? carrier : modulator;
  
//   float sequence[4] = { direct, threshold, window, window_2 };
//   float a = sequence[x_integral];
//   float b = sequence[x_integral + 1];
  
//   return a + (b - a) * x_fractional;
// }
