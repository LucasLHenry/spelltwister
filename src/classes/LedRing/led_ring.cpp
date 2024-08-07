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

void LedRing::begin() {
    btn.attachClick(button_handler);
}

void LedRing::update(int8_t a_change, int8_t b_change) {
    // handle button
    btn.tick();

    // handle encoder
    new_enc_pos = enc.read();
    enc_change = new_enc_pos - enc_pos;
    enc_pos = new_enc_pos;

    if (a_is_active) a_pos_raw += enc_change;
    else b_pos_raw += enc_change;

    if (a_pos_raw < 0) a_pos_raw += 16 << ENC_DIV;
    if (b_pos_raw < 0) b_pos_raw += 16 << ENC_DIV;

    a_idx = (a_pos_raw >> ENC_DIV) % NUM_RING_LEDS;
    b_idx = (b_pos_raw >> ENC_DIV) % NUM_RING_LEDS;

    if (a_change + a_idx < 0) a_change += 16;
    a_idx += a_change;
    if (b_change + b_idx < 0) b_change += 16;
    b_idx += b_change;
}

void button_handler() {
    _LEDRING->a_is_active = !_LEDRING->a_is_active;
}