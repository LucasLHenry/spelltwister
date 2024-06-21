#include "generator.h"

uint16_t calc_upslope(uint16_t rat) {
    if (rat == 0) return 0;
    return max_y / rat;
}

uint16_t calc_downslope(uint16_t rat) {
    if (rat == max_x) return 0;
    return max_y / (max_x - rat);
}

uint16_t asym_lin_map(uint16_t x, uint16_t low, uint16_t mid, uint16_t high) {
    if (x <= 0) return low;
    if (x < half_x) return ((x * (mid - low)) >> (bit_diff << 1)) + low;
    if (x == half_x) return mid;
    if (x > half_x) return (((x - half_x) * (high - mid)) >> (bit_diff << 1)) + mid;
    return high;
}

uint16_t waveform_generator(uint16_t shifted_acc, uint16_t shape, uint16_t ratio, uint16_t upslope, uint16_t downslope) {
    static uint16_t linval, expval, logval, logslope;
    if (shifted_acc <= ratio) {
        linval = upslope * shifted_acc;
        expval = pgm_read_word_near(exptable + (linval >> bit_diff));
        logval = pgm_read_word_near(logtable + (linval >> bit_diff));
    } else {
        linval = downslope * (max_x - shifted_acc);
        expval = pgm_read_word_near(exptable + (linval >> bit_diff));
        logval = pgm_read_word_near(logtable + (linval >> bit_diff));
    }
    return asym_lin_map(shape, expval, linval, logval);
}