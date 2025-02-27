#include <stdint.h>
#include <Adafruit_NeoPXL8.h>
#include <hardware/pwm.h>

#include "../classes/LedRing/led_ring.h"
#include "../classes/Waveformer/waveformer.h"
#include "../classes/NVMWrapper/nvm_wrapper.h"
#include "pins.h"
#include "../tables/colours.h"
#include "../tables/phasors.h"

#ifndef CALIBRATION_H
#define CALIBRATION_H


// need to calibrate the following items:
// 1. volt per octave offset, which determines the minimum value of the pitch knob (should hit
// exactly 30Hz)
// 2. volt per octave scale, which determines the scaling so that the v/o input is actually 1 volt
// per octave
// 3. fm offset, which ensures that a 0 volt input at the fm cv input has no affect on the pitch
// 4. mod offset, which does the same thing as fm offset but for the algorithm modification cv
// 5. shape offset, which is analogous to above for shape
// 6. ratio offset, which is analogous to above for ratio
// 7. encoder direction (some encoders rotate the opposite way of others, can't control this)
//
// these can be calibrated in the following ways:
//    1. set both course and fine time knobs to 0 with no cv input, then read the pitch value
//    2. input 1V, read value, input 3V, read value, do math
//    3, 4, 5, 6. all of these have their own cv inputs so its simply a matter of reading from these
//        while there isn't anything plugged in.
//    7. test encoder direction, click on encoder knob to reverse it
//
//
// this can be achieved in 4 steps:
//      Step 1: with nothing plugged in and all knobs set to full counterclockwise, read cv inputs
//              for v/o, fm, mod, shape, and ratio. These give the offsets for these values
//      Step 2: plug in 1V at v/o input, read to get value 1 for v/o scale
//      Step 3: plug in 3V at v/o input, read to get value 2 for v/o scale
//      Step 4: rotate encoder knob, press encoder knob to reverse direction if needed

void run_calibration(Waveformer& a, Waveformer& b, Adafruit_NeoPXL8& leds, LedRing& ring, NVMWrapper& nvm);
void _calibration_display_startup_leds(Adafruit_NeoPXL8& leds);
void _calibration_wait_for_click(Adafruit_NeoPXL8& leds);
void _calibration_display_module_leds(Adafruit_NeoPXL8& leds, bool is_a, uint16_t voltage);
void _calibration_display_output_leds(Adafruit_NeoPXL8& leds, bool is_a);
void _calibration_do_offset_calibration(Waveformer& wf, ConfigData& conf);
uint16_t _calibration_do_scale_calibration(Waveformer& wf);
bool _calibration_do_encoder_calibration(LedRing& ring, Adafruit_NeoPXL8& leds);
void _calibration_calc_vo_scale_offset(ConfigData* conf);
int16_t _calibration_do_output_offset_calibration(Waveformer& a, Waveformer& b, ConfigData& a_configs, ConfigData& b_configs, bool is_a, bool is_pri);

void _blink_led_non_blocking(Adafruit_NeoPXL8& leds, int led_num, uint32_t colour, uint64_t interval_ms);

#endif