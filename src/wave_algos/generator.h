#include <stdint.h>
#include <avr/pgmspace.h>
#include "../constants.hpp"
#include "../tables/curve_tables.h"

#ifndef GENERATOR_H
#define GENERATOR_H

#define CLIP(x,a,b) (MAX(MIN(x,b),a))

#define CONSTRAIN(var, min, max) \
  if (var < (min)) { \
    var = (min); \
  } else if (var > (max)) { \
    var = (max); \
  }

uint16_t calc_upslope(uint16_t rat);
uint16_t calc_downslope(uint16_t rat);
uint16_t asym_lin_map(uint16_t x, uint16_t low, uint16_t mid, uint16_t high);
uint16_t waveform_generator(uint16_t shifted_acc, uint16_t shape, uint16_t ratio, uint16_t upslope, uint16_t downslope);

#endif