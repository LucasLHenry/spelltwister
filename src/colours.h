#include <Arduino.h>
#include "wave_algos/generator.h"

#ifndef COLOURS_H
#define COLOURS_H

#define ORANGE    0xFF7000
#define RED       0xFF0000
#define WHITE     0xFFFFFF
#define BLACK     0x000000
#define TURQUOISE 0x00FFFF
#define PINK      0xFF00FF
#define PLORANGE  0xBF3880
#define PURPLE    0x7F00FF
#define BLUE      0x0000FF

typedef struct HSL {
    uint16_t h; // 0 to 359
    uint8_t s;  // 0 to 100
    uint8_t l;  // 0 to 100
} HSL;

#define RED_HSL (HSL){0, 100, 50}
#define BLUE_HSL (HSL){240, 100, 50}

constexpr uint32_t pack_colour(uint8_t r, uint8_t g, uint8_t b);
constexpr uint32_t rgb_dim(uint16_t hue, uint8_t value);
constexpr uint32_t hsl_to_packed_rgb(HSL hsl);
constexpr uint8_t hsl_convert(float c, float t1, float t2);
constexpr int generate_brightness_tables(uint16_t a_hue, uint16_t b_hue);

#define BRIGHTNESS_TABLE_LEN 256
extern uint32_t a_side_brightness_table[BRIGHTNESS_TABLE_LEN];
extern uint32_t b_side_brightness_table[BRIGHTNESS_TABLE_LEN];

#endif