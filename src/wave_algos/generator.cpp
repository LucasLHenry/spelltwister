#include "generator.h"

uint16_t calc_upslope(uint16_t rat) {
    if (rat == 0) return 0;
    return max_y / rat;
}

uint16_t calc_downslope(uint16_t rat) {
    if (rat == max_x) return 0;
    return max_y / (max_x - rat);
}

uint16_t waveform_generator(uint16_t shifted_acc, uint16_t shape, uint16_t ratio, uint16_t upslope, uint16_t downslope) {
    uint16_t idx = (shifted_acc <= ratio)? upslope * shifted_acc : downslope * (max_x - shifted_acc);
    uint16_t expval = exptable[idx >> 4]; 
    uint16_t logval = logtable[idx >> 4];
    uint16_t linval = lintable[idx >> 4];

    if (shape <= 0) return expval;
    if (shape < half_x) return ((shape * (linval - expval)) >> (bit_diff << 1)) + expval;
    if (shape == half_x) return linval;
    if (shape > half_x) return (((shape - half_x) * (logval - linval)) >> (bit_diff << 1)) + linval;
    return logval;
}