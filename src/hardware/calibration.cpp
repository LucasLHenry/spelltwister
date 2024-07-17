#include "calibration.h"

void run_calibration(Waveformer& a, Waveformer& b, Adafruit_NeoPXL8& leds, NVMWrapper& nvm) {
    _calibration_display_startup_leds(leds);
    ConfigData a_configs, b_configs;


    /****************STEP ONE************************/
    _calibration_display_module_leds(leds, true, ONE);
    _calibration_wait_for_click();
    _calibration_do_offset_calibration(a, a_configs);

    _calibration_display_module_leds(leds, false, ONE);
    _calibration_wait_for_click();
    _calibration_do_offset_calibration(b, b_configs);

    /*****************STEP TWO**********************/
    uint16_t a_val_1v, b_val_1v, a_val_3v, b_val_3v;
    _calibration_display_module_leds(leds, true, TWO);
    _calibration_wait_for_click();
    a_val_1v = _calibration_do_scale_calibration(a);

    _calibration_display_module_leds(leds, false, TWO);
    _calibration_wait_for_click();
    b_val_1v = _calibration_do_scale_calibration(b);


    /*******************STEP THREE*******************/
    _calibration_display_module_leds(leds, true, THREE);
    _calibration_wait_for_click();
    a_val_3v = _calibration_do_scale_calibration(a);

    _calibration_display_module_leds(leds, false, THREE);
    _calibration_wait_for_click();
    b_val_3v = _calibration_do_scale_calibration(b);

    a_configs.vo_scale = _calibration_calc_vo_scale(a_val_1v, a_val_3v);
    b_configs.vo_scale = _calibration_calc_vo_scale(b_val_1v, b_val_3v);

    nvm.write_config_data(true,  a_configs);
    nvm.write_config_data(false, b_configs);
    nvm.save_data();
}

void _calibration_wait_for_click() {
    while(true) {
        if (digitalRead(FLW_BTN) == HIGH) return;
    }
}

void _calibration_display_module_leds(Adafruit_NeoPixel& leds, bool is_a, _Step step) {
    leds.fill(BLACK);
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
        leds.setPixelColor(PRI_A_LED,  RED);
        leds.setPixelColor(SEC_A_LED,  RED);
        leds.setPixelColor(TRIG_A_LED, RED);
        for (int i = 0; i < amt_ring_leds; i++) {
            leds.setPixelColor(i, RED);
        }
    } else {
        leds.setPixelColor(PRI_B_LED,  BLUE);
        leds.setPixelColor(SEC_B_LED,  BLUE);
        leds.setPixelColor(TRIG_B_LED, BLUE);
        for (int i = 0; i < amt_ring_leds; i++) {
            leds.setPixelColor(NUM_LEDS-1 - i, BLUE);
        }
    }
    leds.show();
}

void _calibration_display_startup_leds(Adafruit_NeoPXL8& leds) {
    leds.fill(BLACK);
    for (int repeat = 0; repeat < 2; repeat++) {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            for (int j = 0; j < NUM_RING_LEDS; j++) {
                uint8_t colour = (i==j)? RED : ((i==(j+1)%NUM_RING_LEDS)? BLUE : BLACK);
                leds.setPixelColor(j, colour);
            }
            leds.show();
            delay(100);
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
}

uint16_t _calibration_do_scale_calibration(Waveformer& wf) {
    AllInputs vals = wf.get_all(16);
    return vals.pitch;
}

uint16_t _calibration_calc_vo_scale(uint16_t one_volt, uint16_t three_volts) {
    return static_cast<uint16_t>((1 << 18) / (one_volt - three_volts));
}