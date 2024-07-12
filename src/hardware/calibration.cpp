#include "calibration.h"

void run_calibration(Waveformer& a, Waveformer& b, Adafruit_NeoPXL8& leds, NVMWrapper& nvm) {
    _calibration_display_startup_leds(leds);
    ConfigData a_configs, b_configs;
    _calibration_do_scale_calibration(a, a_configs);
    _calibration_do_scale_calibration(b, b_configs);
}

void _calibration_display_startup_leds(Adafruit_NeoPXL8& leds) {
    for (int repeat = 0; repeat < 2) {
        for (int i = 0; i < NUM_RING_LEDS; i++) {
            for (int j = 0; j < NUM_RING_LEDS; j++) {
                uint8_t colour = (i==j)? RED : ((i==(j+1)%NUM_RING_LEDS)? BLUE : BLACK);
                leds.setPixelColour(j, colour);
            }
            leds.show();
            delay(100);
        }
    }
}

void _calibration_do_scale_calibration(Waveformer& wf, ConfigData& conf) {
    AllInputs zero_vals = wf.read_all(16);
    conf.vo_offset = zero_vals.pitch;
    conf.fm_offset = zero_vals.fm;
    conf.shape_offset = zero_vals.shape_cv;
    conf.ratio_offset = zero_vals.ratio_cv;
}