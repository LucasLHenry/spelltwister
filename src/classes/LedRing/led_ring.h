#include <stdint.h>
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include <OneButton.h>
#include <Adafruit_NeoPXL8.h>

#include "../../hardware/pins.h"
#include "../../constants.hpp"
#include "../../tables/colours.h"

#ifndef LED_RING_H
#define LED_RING_H

#define ENC_DIV 2

class LedRing {
    int64_t enc_pos, new_enc_pos;
    int64_t enc_change;
    Encoder enc;
    public:
        bool calibration_mode;
        bool reversed;
        OneButton btn;
        int16_t a_idx, b_idx;
        int16_t a_idx_wo_cv, b_idx_wo_cv;
        int64_t a_pos_raw, b_pos_raw;
        bool a_is_active;
        void write_leds(Adafruit_NeoPXL8 &leds);
        void begin(uint8_t saved_a, uint8_t saved_b);
        void update(int8_t a_change, int8_t b_change);
        LedRing(int enc_pin_1, int enc_pin_2, int btn_pin);

};

void button_handler();

extern LedRing* _LEDRING;

#endif