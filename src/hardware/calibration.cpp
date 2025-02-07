#include "calibration.h"

void run_calibration(Waveformer& a, Waveformer& b, Adafruit_NeoPXL8& leds, LedRing& ring, NVMWrapper& nvm) {
    _calibration_display_startup_leds(leds);
    ConfigData a_configs, b_configs;


    /****************STEP ONE************************/
    _calibration_display_module_leds(leds, true, 0);
    _calibration_wait_for_click(leds);
    _calibration_do_offset_calibration(a, a_configs);

    _calibration_display_module_leds(leds, false, 0);
    _calibration_wait_for_click(leds);
    _calibration_do_offset_calibration(b, b_configs);

    for (uint16_t i = 1; i < NUM_SCALE_VOLTAGES+1; i++) {
        _calibration_display_module_leds(leds, true, i);
        _calibration_wait_for_click(leds);
        a_configs.vo_margins[i] = _calibration_do_scale_calibration(a);

        _calibration_display_module_leds(leds, false, i);
        _calibration_wait_for_click(leds);
        b_configs.vo_margins[i] = _calibration_do_scale_calibration(b);
    }


    /******************STEP FOUR*********************/
    bool reversed = _calibration_do_encoder_calibration(ring, leds);

    /******************CALCULATIONS******************/
    _calibration_calc_vo_scale_offset(&a_configs);
    _calibration_calc_vo_scale_offset(&b_configs);

    /******************SAVE DATA*********************/
    a.configs = a_configs;
    b.configs = b_configs;
    ring.reversed = reversed;
    nvm.set_config_data(true, a_configs);
    nvm.set_config_data(false, b_configs);
    nvm.set_encoder_direction(reversed);
    nvm.save_config_data();
}

void _calibration_do_offset_calibration(Waveformer& wf, ConfigData& conf) {
    AllInputs zero_vals = wf.get_all(64);
    conf.vo_margins[0] = zero_vals.pitch;
    conf.fm_offset = zero_vals.fm;
    conf.shp_cv_offset = zero_vals.shape_cv;
    conf.rat_cv_offset = zero_vals.ratio_cv;
    conf.shp_pot_offset = zero_vals.shape_pot;
    conf.rat_pot_offset = zero_vals.ratio_pot;
    conf.mod_offset = zero_vals.algo_mod;
}

uint16_t _calibration_do_scale_calibration(Waveformer& wf) {
    AllInputs vals = wf.get_all(64);
    return vals.pitch;
}

bool _reversed;
bool _encoder_dir_changed;
void _calibration_encoder_btn_handler() {
    _reversed = !_reversed;
    _encoder_dir_changed = true;
}

bool _calibration_do_encoder_calibration(LedRing& ring, Adafruit_NeoPXL8& leds) {
    ring.btn.attachClick(_calibration_encoder_btn_handler);
    ring.a_is_active = true;
    ring.calibration_mode = true;
    while (true) {
        if (digitalRead(FLW_BTN) == HIGH) break;
        if (_encoder_dir_changed) {
            ring.reversed = _reversed;
            _encoder_dir_changed = false;
        }
        leds.fill(black);
        ring.update(0, 0);
        ring.write_leds(leds);
        _blink_led_non_blocking(leds, FLW_LED, mix_colour, 500);
    }
    ring.calibration_mode = false;
    return _reversed;
}

void _calibration_calc_vo_scale_offset(ConfigData* conf) {
    int32_t b_i, m_i, m_i_minus_1, b_i_minus_1;
    for (uint16_t i = 0; i < NUM_SCALE_VOLTAGES; i++) {
        m_i = scale_factor / (conf->vo_margins[i+1] - conf->vo_margins[i]);
        if (i == 0) {
            b_i = conf->vo_margins[0] * ((1<<vo_upsample_amt) - m_i);
        } else {
            b_i = b_i_minus_1 - (m_i - m_i_minus_1)*conf->vo_margins[i-1];
        }
        conf->vo_scale[i] = m_i;
        conf->vo_offset[i] = b_i;
        m_i_minus_1 = m_i;
        b_i_minus_1 = b_i;
    }
    for (uint16_t i = 0; i < NUM_SCALE_VOLTAGES; i++) {
        conf->vo_offset[i] -= conf->vo_margins[0] << vo_upsample_amt;
    }
}

void _calibration_wait_for_click(Adafruit_NeoPXL8& leds) {
    while(true) {
        if (digitalRead(FLW_BTN) == HIGH) break;
        _blink_led_non_blocking(leds, FLW_LED, mix_colour, 500);
    }
    delay(700);
}

void _blink_led_non_blocking(Adafruit_NeoPXL8& leds, int led_num, uint32_t colour, uint64_t interval_ms) {
    static uint64_t _previous_ms, _current_ms;
    static bool _led_state;
    _current_ms = millis();
    if (_current_ms - _previous_ms >= interval_ms) {
        _led_state = !_led_state;
        _previous_ms = _current_ms;
        leds.setPixelColor(led_num, (_led_state)? colour : black);
    }
    leds.show();
}

void _calibration_display_module_leds(Adafruit_NeoPXL8& leds, bool is_a, uint16_t voltage) {
    leds.fill(black);

    if (is_a) {
        leds.setPixelColor(PRI_A_LED,  a_colour);
        leds.setPixelColor(SEC_A_LED,  a_colour);
        leds.setPixelColor(TRIG_A_LED, a_colour);
        for (int i = 0; i < voltage; i++) {
            leds.setPixelColor(i, a_colour);
        }
    } else {
        leds.setPixelColor(PRI_B_LED,  b_colour);
        leds.setPixelColor(SEC_B_LED,  b_colour);
        leds.setPixelColor(TRIG_B_LED, b_colour);
        for (int i = 0; i < voltage; i++) {
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