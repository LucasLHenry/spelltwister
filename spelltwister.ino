/*
Developed by Lucas Henry, May 2024

The main entry point file for the firmware, showing high-level logic.
*/

#include <Arduino.h>
#include <Adafruit_NeoPXL8.h>
#include <hardware/pwm.h>

#include "src/classes/Waveformer/waveformer.h"
#include "src/classes/LedRing/led_ring.h"
#include "src/classes/Modulator/modulator.h"
#include "src/classes/NVMWrapper/nvm_wrapper.h"
#include "src/hardware/pins.h"
#include "src/hardware/misc_functions.h"
#include "src/hardware/calibration.h"
#include "src/constants.hpp"
#include "src/wave_algos/generator.h"
#include "src/wave_algos/mod_algorithms.h"
#include "src/tables/colours.h"


// --- SELECTED RING ALGORITHMS ---
// on the front panel, runs clockwise starting at the bottom
algo_f_ptr algo_arr[16] = {
    rectify,                    // single   // similar to double freq for VCO
    half_freq,                  // single   // good
    double_freq,                // single   // good
    invert,                     // single   // good for envelopes
    exclusive_or,               // double   // really good
    difference,                 // double   // needed with sum? only relevant for envelopes
    sum,                        // double   // good
    frequency_mod,              // double   // good
    amplitude_mod,              // double   // good
    gate,                       // double   // meh, but should probably stay
    shape_mod,                  // double   // good
    ratio_mod,                  // double   // good
    sample_and_hold,
    // sine_pm,                    // single   // good but maybe bumps with CZ101 idea
    sample_rate_reduce,         // single   //
    analog_pulse_pm,            // single   //
    rungle                  // double   // good
};

// --- GLOBAL VARIABLES ---
int8_t led_pins[8] = LED_DATA;  // pin setup for led strip
Adafruit_NeoPXL8 leds(NUM_LEDS, led_pins, NEO_GRB);  // led strip controller
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);  // algorithm ring controller
LedRing* _LEDRING = &ring; // used for internal ISR stuff
OneButton follow_btn(FLW_BTN, false, false);  // follow button

Waveformer a(true,  MUX_A, LIN_TIME_A);  // left side waveform generator
Waveformer b(false, MUX_B, LIN_TIME_B);  // right side waveform generator
Modulator mod_a(a, b, ring, algo_arr);  // left side waveform modulator
Modulator mod_b(b, a, ring, algo_arr);  // right side waveform modulator

NVMWrapper nvm;  // interface for loading/storing config data

bool save_mod_pos_flag;  // flag raised when user triggers encoder position save
uint16_t a_pwm_slice, b_pwm_slice;  // hardware PWM slices allocated to A and B
bool calibrating = false;  // indicates code is in a calibration state
repeating_timer_t pwm_timer;

// --- FORWARD DECLARATIONS ---
void encoder_long_press_ISR();
void follow_ISR();
bool pwm_timer_ISR(repeating_timer_t* rt);
void a_sync_ISR();
void b_sync_ISR();
void write_other_leds();

// --- CORE 0 SETUP/LOOP ---
void setup() {
    analogReadResolution(BITS_ADC);
    Serial.begin(9600);

    // initialize objects
    a.init(&b);
    b.init(&a);
    nvm = NVMWrapper();
    a.configs = nvm.get_config_data(true);
    b.configs = nvm.get_config_data(false);
    ring.reversed = nvm.get_encoder_direction();
    leds.begin();
    leds.setBrightness(0x70);

    // calibration mode
    if (digitalRead(FLW_BTN) == HIGH) {
        calibrating = true;
        run_calibration(a, b, leds, ring, nvm);
        calibrating = false;
    }

    ring.btn.attachLongPressStart(encoder_long_press_ISR);
    ring.btn.setPressMs(3000);
    ring.begin(nvm.get_mod_pos(true), nvm.get_mod_pos(false));

    follow_btn.attachClick(follow_ISR);
}

void loop() {
    // save state if triggered
    if (save_mod_pos_flag) {
        blink(leds, mix_colour, 50, 300);
        nvm.set_mod_pos(true, ring.a_idx_wo_cv);
        nvm.set_mod_pos(false, ring.b_idx_wo_cv);
        nvm.save_mod_pos();
        pwm_set_enabled(a_pwm_slice, true);
        pwm_set_enabled(b_pwm_slice, true);
        save_mod_pos_flag = false;
    }

    // read inputs
    a.read();
    b.read();
    follow_btn.tick();
    ring.update(a.mod_idx, b.mod_idx);
    
    ring.write_leds(leds);
    write_other_leds();
    leds.show();
}


// --- CORE 1 SETUP/LOOP ---
void setup1() {
    // setup outputs
    constexpr uint16_t max_val = (1 << BIT_DEPTH) - 1;
    a_pwm_slice = setup_pwm_pins(PRI_OUT_A, SEC_OUT_A, max_val);
    b_pwm_slice = setup_pwm_pins(PRI_OUT_B, SEC_OUT_B, max_val);
    pinMode(TRIG_OUT_A, OUTPUT);
    pinMode(TRIG_OUT_B, OUTPUT);

    // timer setup
    int64_t timer_period_us = - (1000 / PWM_FREQ_kHz);  // negative means include execution time in period
    alarm_pool_t* pool = alarm_pool_create(0, 1);
    alarm_pool_add_repeating_timer_us(pool, timer_period_us, pwm_timer_ISR, NULL, &pwm_timer);

    // interrupt setup
    attachInterrupt(digitalPinToInterrupt(SIG_IN_A), a_sync_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(SIG_IN_B), b_sync_ISR, FALLING);
}

void loop1() {}


// --- OTHER FUNCTIONS ---

bool pwm_timer_ISR(repeating_timer_t* rt) {
    if (calibrating) return true;
    // increment phasors etc.
    a.update();
    b.update();

    // create new outputs
    a.generate();
    b.generate();
    mod_a.generate();
    mod_b.generate();

    // push new values to outputs
    // subtract max_x to account for filter signal inversion
    pwm_set_gpio_level(PRI_OUT_A, max_x - (a.val >> bit_diff));
    pwm_set_gpio_level(PRI_OUT_B, max_x - (b.val >> bit_diff));
    pwm_set_gpio_level(SEC_OUT_A, max_x - (mod_a.val >> bit_diff));
    pwm_set_gpio_level(SEC_OUT_B, max_x - (mod_b.val >> bit_diff));
    gpio_put(TRIG_OUT_A, !a.end_of_cycle);
    gpio_put(TRIG_OUT_B, !b.end_of_cycle);
    return true;
}

void write_other_leds() {
    uint8_t a_brightness = (a.val < half_y)? 0 : a.val - half_y >> 7;
    uint8_t a_mod_brightness = (mod_a.val < half_y)? 0 : mod_a.val - half_y >> 7;

    uint8_t b_brightness = (b.val < half_y)? 0 : b.val - half_y >> 7;
    uint8_t b_mod_brightness = (mod_b.val < half_y)? 0 : mod_b.val - half_y >> 7;

    // write signal indicators
    leds.setPixelColor(PRI_A_LED, a_brightness_table[a_brightness]);
    leds.setPixelColor(SEC_A_LED, a_brightness_table[a_mod_brightness]);
    leds.setPixelColor(PRI_B_LED, b_brightness_table[b_brightness]);
    leds.setPixelColor(SEC_B_LED, b_brightness_table[b_mod_brightness]);

    // write trig LEDs
    leds.setPixelColor(TRIG_A_LED, (a.eos_led)? a_colour : black);
    leds.setPixelColor(TRIG_B_LED, (b.eos_led)? b_colour : black);

    // write follow LED
    leds.setPixelColor(FLW_LED, (b.follow_mode != DISABLED)? mix_colour : black);
}

// responsible for triggering saving code
void encoder_long_press_ISR() {
    save_mod_pos_flag = true;
    pwm_set_enabled(a_pwm_slice, false);
    pwm_set_enabled(b_pwm_slice, false);
}

// called when the follow button is pressed
void follow_ISR() {
    b.toggle_follow_mode();
}

// called when rising input trigger detected on A sync
void a_sync_ISR() {
    a.reset();
    a.running = true;
    if (b.follow_mode != DISABLED) b_sync_ISR();
}

// called when rising input trigger detected on B sync
void b_sync_ISR() {
    b.reset();
    b.running = true;
}