#include "calibration.h"

void run_calibration(Waveformer& a, Waveformer& b, Adafruit_NeoPXL8& leds, NVMWrapper& nvm) {
    _calibration_display_startup_leds(leds);
    ConfigData a_configs, b_configs;


    /****************STEP ONE************************/
    _calibration_display_module_leds(leds, true, ONE);
    _calibration_wait_for_click(leds);
    _calibration_do_offset_calibration(a, a_configs);

    _calibration_display_module_leds(leds, false, ONE);
    _calibration_wait_for_click(leds);
    _calibration_do_offset_calibration(b, b_configs);

    /*****************STEP TWO**********************/
    uint16_t a_val_1v, b_val_1v, a_val_3v, b_val_3v;
    _calibration_display_module_leds(leds, true, TWO);
    _calibration_wait_for_click(leds);
    a_val_1v = _calibration_do_scale_calibration(a);

    _calibration_display_module_leds(leds, false, TWO);
    _calibration_wait_for_click(leds);
    b_val_1v = _calibration_do_scale_calibration(b);


    /*******************STEP THREE*******************/
    _calibration_display_module_leds(leds, true, THREE);
    _calibration_wait_for_click(leds);
    a_val_3v = _calibration_do_scale_calibration(a);

    _calibration_display_module_leds(leds, false, THREE);
    _calibration_wait_for_click(leds);
    b_val_3v = _calibration_do_scale_calibration(b);

    a_configs.vo_scale  = _calibration_calc_vo_scale(a_val_1v, a_val_3v);
    a_configs.vo_offset = _calibration_calc_vo_offset(a_configs.vo_scale, a_configs.vo_offset);
    b_configs.vo_scale  = _calibration_calc_vo_scale(b_val_1v, b_val_3v);
    b_configs.vo_offset = _calibration_calc_vo_offset(b_configs.vo_scale, b_configs.vo_offset);

    // Serial.println("A CONFIGS");
    // print_config_data(a_configs);
    // Serial.println("B CONFIGS");
    // print_config_data(a_configs);

    a.configs = a_configs;
    b.configs = b_configs;
    nvm.set_config_data(true, a_configs);
    nvm.set_config_data(false, b_configs);
    nvm.save_config_data();
}

void _calibration_wait_for_click(Adafruit_NeoPXL8& leds) {
    while(true) {
        if (digitalRead(FLW_BTN) == HIGH) break;
        _blink_led_non_blocking(leds, FLW_LED, mix_colour, 500);
    }
    delay(500);
}

void _calibration_display_module_leds(Adafruit_NeoPXL8& leds, bool is_a, _Step step) {
    leds.fill(black);
    uint8_t amt_ring_leds = 0;
    switch (step) {
        case ONE:
            amt_ring_leds = 0;
            break;
        case TWO:
            amt_ring_leds = 1;
            break;
        case THREE:
            amt_ring_leds = 3;
            break;
    }

    if (is_a) {
        leds.setPixelColor(PRI_A_LED,  a_colour);
        leds.setPixelColor(SEC_A_LED,  a_colour);
        leds.setPixelColor(TRIG_A_LED, a_colour);
        for (int i = 0; i < amt_ring_leds; i++) {
            leds.setPixelColor(i, a_colour);
        }
    } else {
        leds.setPixelColor(PRI_B_LED,  b_colour);
        leds.setPixelColor(SEC_B_LED,  b_colour);
        leds.setPixelColor(TRIG_B_LED, b_colour);
        for (int i = 0; i < amt_ring_leds; i++) {
            leds.setPixelColor((NUM_RING_LEDS - i)%NUM_RING_LEDS, b_colour);
        }
    }
    leds.show();
}

void _calibration_display_startup_leds(Adafruit_NeoPXL8& leds) {
    leds.fill(black);
    for (int repeat = 0; repeat < 1; repeat++) {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            for (int j = 0; j < NUM_RING_LEDS; j++) {
                uint32_t colour;
                switch ((i - j) % 16) {
                    case  3: colour = a_brightness_table[220]; break;
                    case  2: colour = a_brightness_table[150]; break;
                    case  1: colour = a_colour; break;
                    case  0: colour = mix_colour; break;
                    case -1: colour = b_colour; break;
                    case -2: colour = b_brightness_table[150]; break;
                    case -3: colour = b_brightness_table[220]; break;
                    default: colour = black;
                }
                leds.setPixelColor(j, colour);
            }
            leds.show();
            delay(75);
        }
    }
}

void _calibration_do_offset_calibration(Waveformer& wf, ConfigData& conf) {
    AllInputs zero_vals = wf.get_all(16);
    conf.vo_offset = zero_vals.pitch;
    conf.fm_offset = zero_vals.fm;
    conf.shp_cv_offset = zero_vals.shape_cv;
    conf.rat_cv_offset = zero_vals.ratio_cv;
    conf.shp_pot_offset = zero_vals.shape_pot;
    conf.rat_pot_offset = zero_vals.ratio_pot;
    conf.mod_offset = zero_vals.algo_mod;
}

uint16_t _calibration_do_scale_calibration(Waveformer& wf) {
    AllInputs vals = wf.get_all(16);
    return vals.pitch;
}

uint16_t _calibration_calc_vo_scale(uint16_t one_volt, uint16_t three_volts) {
    return static_cast<uint16_t>((scale_factor) / static_cast<uint32_t>(one_volt - three_volts));
}

uint16_t _calibration_calc_vo_offset(uint16_t vo_scale, uint16_t zero_volts) {
    return (zero_volts * vo_scale) >> 8;
}


void _blink_led_non_blocking(Adafruit_NeoPXL8& leds, int led_num, uint32_t colour, uint64_t interval_ms) {
    static uint64_t _previous_ms, _current_ms;
    static bool _led_state;
    _current_ms = millis();
    if (_current_ms - _previous_ms >= interval_ms) {
        _led_state = !_led_state;
        _previous_ms = _current_ms;
        leds.setPixelColor(led_num, (_led_state)? colour : black);
        leds.show();
    }
}