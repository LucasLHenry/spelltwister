#include "led_ring.h"

LedRing::LedRing(int enc_pin_1, int enc_pin_2, int btn_pin):
    enc(enc_pin_1, enc_pin_2),
    btn(btn_pin, true, true),
    a_is_active(true)
{
    pinMode(enc_pin_1, INPUT_PULLUP);
    pinMode(enc_pin_2, INPUT_PULLUP);
}

void LedRing::write_leds(Adafruit_NeoPXL8 &leds) {
    if (calibration_mode) {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            if (i == a_idx) leds.setPixelColor(i, mix_colour);
            else leds.setPixelColor(i, black);
        }
        return;
    }
    
    if (a_idx == b_idx) {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            if (i == a_idx) leds.setPixelColor(i, mix_colour);
            else leds.setPixelColor(i, black);
        }
    } else {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            if (i == a_idx) leds.setPixelColor(i, a_colour);
            else if (i == b_idx) leds.setPixelColor(i, b_colour);
            else leds.setPixelColor(i, black);
        }
    }
}

void LedRing::begin(uint8_t saved_a, uint8_t saved_b) {
    btn.attachClick(button_handler);
    a_pos_raw = static_cast<uint16_t>(saved_a) << ENC_DIV;
    b_pos_raw = static_cast<uint16_t>(saved_b) << ENC_DIV;
    a_idx_wo_cv = saved_a;
    b_idx_wo_cv = saved_b;
}

void LedRing::update(int8_t a_change, int8_t b_change) {
    // handle button
    btn.tick();

    // handle encoder
    new_enc_pos = enc.read();
    enc_change = (reversed)? enc_pos - new_enc_pos : new_enc_pos - enc_pos;
    enc_pos = new_enc_pos;

    if (a_is_active) a_pos_raw += enc_change;
    else b_pos_raw += enc_change;

    if (a_pos_raw < 0) a_pos_raw += 16 << ENC_DIV;
    if (b_pos_raw < 0) b_pos_raw += 16 << ENC_DIV;

    a_idx_wo_cv = (a_pos_raw >> ENC_DIV) % NUM_RING_LEDS;
    b_idx_wo_cv = (b_pos_raw >> ENC_DIV) % NUM_RING_LEDS;


    a_idx = a_idx_wo_cv;
    b_idx = b_idx_wo_cv;
    if (a_change + a_idx < 0) a_change += 16;
    a_idx += a_change;
    if (b_change + b_idx < 0) b_change += 16;
    b_idx += b_change;
}

void button_handler() {
    _LEDRING->a_is_active = !_LEDRING->a_is_active;
}