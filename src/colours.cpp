#include "colours.h"

constexpr uint32_t pack_colour(uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) <<  8) | b;
}

constexpr uint32_t rgb_dim(uint16_t hue, uint8_t value) {
    HSL hsl = {hue, 100, value};
    return hsl_to_packed_rgb(hsl);
}

constexpr uint32_t hsl_to_packed_rgb(HSL hsl) {
    float h=0, s=0, l=0, t1=0, t2=0, tr=0, tg=0, tb=0;
    uint8_t r=0, g=0, b=0;

    h = (hsl.h % 360) / 360.0;
    s = CLIP(hsl.s, 0, 100) / 100.0;
    l = CLIP(hsl.l, 0, 100) / 100.0;

    if ( s == 0 ) { 
        r = g = b = 255 * l;
        return pack_colour(r, g, b);
    } 

    if ( l < 0.5 ) t1 = l * (1.0 + s);
    else t1 = l + s - l * s;

    t2 = 2 * l - t1;
    tr = h + 1/3.0;
    tg = h;
    tb = h - 1/3.0;

    r = hsl_convert(tr, t1, t2);
    g = hsl_convert(tg, t1, t2);
    b = hsl_convert(tb, t1, t2);

    return pack_colour(r, g, b);
}


constexpr uint8_t hsl_convert(float c, float t1, float t2) {
    if ( c < 0 ) c+=1; 
    else if ( c > 1 ) c-=1;

    if ( 6 * c < 1 ) c = t2 + ( t1 - t2 ) * 6 * c;
    else if ( 2 * c < 1 ) c = t1;
    else if ( 3 * c < 2 ) c = t2 + ( t1 - t2 ) * ( 2/3.0 - c ) * 6;
    else c = t2;

    return static_cast<uint8_t>(c*255); 
}

constexpr int generate_brightness_tables(uint16_t a_hue, uint16_t b_hue) {
    // a_side_brightness_table
    // b_side_brightness_table
    for (int i = 0; i < BRIGHTNESS_TABLE_LEN; i++) {
        uint8_t brightness = 50 * static_cast<double>(i) / 255;
        a_side_brightness_table[i] = rgb_dim(a_hue, brightness);
        b_side_brightness_table[i] = rgb_dim(b_hue, brightness);
    }
    return 0;
}