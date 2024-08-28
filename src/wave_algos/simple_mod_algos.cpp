#include "simple_mod_algos.h"

uint16_t _sum(uint16_t v1, uint16_t v2) {
    return (v1 + v2) >> 1;
}

uint16_t _difference(uint16_t v1, uint16_t v2) {
    return (v1 - v2 + max_y) >> 1;
}

uint16_t _exclusive_or(uint16_t v1, uint16_t v2) {
    uint16_t top = v1 & 0xE000;
    uint16_t bottom = (v1 & 0x1FFF) ^ (v2 & 0x1FFF);
    return top | bottom;
}

uint16_t _invert(uint16_t v1) {
    // FIXME only useful for LFO and ENV
    return max_y - v1;
}

uint16_t _analog_pulse_pm(uint16_t v1) {
    // FIXME square wave lookup table isn't that interesting
    return analog_pulse_table[v1 >> 5];
}

uint16_t _rectify(uint16_t v1) {
    // FIXME maybe apply a litte saturation to this as well? Could be interesting
    uint32_t val = (v1 < half_y)? max_y - v1 : v1;
    return static_cast<uint16_t>((val << 1) - max_y);
}

uint16_t _bitcrush(uint16_t v1) {
    return (v1 >> CRUSH_AMT) << CRUSH_AMT;
}

uint16_t _sine_pm(uint16_t v1) {
    // FIXME this should do more, not be just a sine wave when the waveformer generates a saw
    return sine_table[v1 >> 5];
}

uint16_t _gate(uint16_t v1, uint16_t v2) {
    if (v2 <= half_y) return 0;
    return v2;
}

uint16_t _amplitude_mod(uint16_t v1, uint16_t v2) {
    // FIXME this should account for ENV vs LFO / VCO, respond differently
    return static_cast<uint16_t>((((static_cast<int64_t>(v1) - half_y) * (static_cast<int64_t>(v2) - half_y)) >> 15) + half_y);
}

uint16_t _ring_modulate(uint16_t v1, uint16_t v2) {
    // FIXME ring mod and AM? might bump a little
    return static_cast<uint16_t>((v1 * static_cast<uint64_t>(v2)) >> 16);
}