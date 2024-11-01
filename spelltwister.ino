/*
Developed by Lucas Henry, May 2024

This file is meant to be as readable as possible, showing only high-level logic. All of the meat of this
project is within the src directory, so dig into that to do your hacking.
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


/*
RP2040 is a dual core processor. These cores are referred to as c0 and c1. c0 calls setup() and loop(),
while c1 calls setup1() and loop1(). In this implementation, the two cores are configured to perform 
different tasks. 

c0 handles non time-critical input sensing, which is pretty much every input except
for trig/sync in. It does so by reading from the ADCs in the loop() function, and updating the correct
values in the global variables. 

c1 is responsible for all of the time-critical applications, which means generating waveforms and handling
trig/sync in. Waveform samples are generated at approximately 48kHz, using a PWM carrier waveform of ~60kHz
with an output bit depth of 11 (range of 0 to 2047). Because of this, the sample generation function is not
called within loop1(), it is called based on a hardware timer. Therefore, loop1() is completely empty.
*/

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
    sine_pm,                    // single   // good but maybe bumps with CZ101 idea
    sample_rate_reduce,         // single   //
    analog_pulse_pm,            // single   //
    crosscrush                  // double   // good
};


int8_t led_pins[8] = LED_DATA;
Adafruit_NeoPXL8 leds(NUM_LEDS, led_pins, NEO_GRB);  // led strip controller
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);  // algorithm ring controller
LedRing* _LEDRING = &ring; // used for internal ISR stuff
OneButton follow_btn(FLW_BTN, false, false);  // button in center

Waveformer a(true,  MUX_A, LIN_TIME_A);
Waveformer b(false, MUX_B, LIN_TIME_B);
Modulator mod_a(a, b, ring, algo_arr);
Modulator mod_b(b, a, ring, algo_arr);

NVMWrapper nvm;

// called when the follow button is pressed
void follow_ISR() {
    b.follow = !b.follow;
    if (b.follow) {
        b.core.set(a.core);
    }
}

bool calibrating = false;
void setup() {
    analogReadResolution(BITS_ADC);
    Serial.begin(9600);

    // initialize objects
    a.init(&b);
    b.init(&a);
    nvm = NVMWrapper();
    ring.begin(nvm.get_mod_pos(true), nvm.get_mod_pos(false));
    a.configs = nvm.get_config_data(true);
    b.configs = nvm.get_config_data(false);
    leds.begin();
    leds.setBrightness(0x70);

    // calibration mode
    if (digitalRead(FLW_BTN) == HIGH) {
        calibrating = true;
        run_calibration(a, b, leds, nvm);
        calibrating = false;
    }

    follow_btn.attachClick(follow_ISR);

}

void write_other_leds();


uint64_t loop_counter, runtime_s;
bool save_mod_pos_flag;
void loop() {
    // read inputs
    a.read();
    b.read();
    follow_btn.tick();

    ring.update(a.mod_idx, b.mod_idx);
    ring.write_leds(leds);
    write_other_leds();
    leds.show();

    loop_counter++;
    if (loop_counter % LOOPS_PER_SEC == 0) {
        runtime_s++;
        if (runtime_s % 20 == 0) {
            nvm.set_mod_pos(true, ring.a_idx_wo_cv);
            nvm.set_mod_pos(false, ring.b_idx_wo_cv);
            save_mod_pos_flag = true;
        }
    }
}

repeating_timer_t pwm_timer;
bool PwmTimerHandler(repeating_timer_t* rt);

void a_sync_ISR() {
    a.reset();
    a.running = true;
}

void b_sync_ISR() {
    b.reset();
    b.running = true;
}

void setup1() {
    // setup outputs!
    constexpr uint16_t max_val = (1 << BIT_DEPTH) - 1;
    constexpr uint32_t pwm_freq = CLOCK_FREQ / max_val;

    setup_pwm_pins(PRI_OUT_A, SEC_OUT_A, max_val);
    setup_pwm_pins(PRI_OUT_B, SEC_OUT_B, max_val);

    pinMode(TRIG_OUT_A, OUTPUT);
    pinMode(TRIG_OUT_B, OUTPUT);

    // timer setup
    int64_t timer_period_us = - (1000 / PWM_FREQ_kHz);
    alarm_pool_t* pool = alarm_pool_create(0, 1);
    alarm_pool_add_repeating_timer_us(pool, timer_period_us, PwmTimerHandler, NULL, &pwm_timer);

    // interrupt setup
    attachInterrupt(digitalPinToInterrupt(SIG_IN_A), a_sync_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(SIG_IN_B), b_sync_ISR, FALLING);
}

void loop1() {
    // have to do this from core 1 so that the outputs don't drop out
    if (save_mod_pos_flag) {
        nvm.save_mod_pos();
        save_mod_pos_flag = false;
    }
}

bool PwmTimerHandler(repeating_timer_t* rt) {
    if (calibrating) return true;

    a.update();
    b.update();

    a.generate();
    b.generate();
    mod_a.generate();
    mod_b.generate();
    // subtract max_x to account for output filter inversion
    pwm_set_gpio_level(PRI_OUT_A, max_x - (a.val >> bit_diff));
    pwm_set_gpio_level(PRI_OUT_B, max_x - (b.val >> bit_diff));
    pwm_set_gpio_level(SEC_OUT_A, max_x - (mod_a.val >> bit_diff));
    pwm_set_gpio_level(SEC_OUT_B, max_x - (mod_b.val >> bit_diff));
    gpio_put(TRIG_OUT_A, !a.end_of_cycle);
    gpio_put(TRIG_OUT_B, !b.end_of_cycle);
    return true;
}

void write_other_leds() {
    uint8_t a_brightness, a_mod_brightness, b_brightness, b_mod_brightness;

    if (a.mode == ENV) {
        a_brightness = (a.val - half_y) >> 7;
        a_mod_brightness = (mod_a.val - half_y) >> 7;
    } else {
        a_brightness = a.val >> 8;
        a_mod_brightness = mod_a.val >> 8;
    }

    if (b.mode == ENV) {
        b_brightness = (b.val - half_y) >> 7;
        b_mod_brightness = (mod_b.val - half_y) >> 7;
    } else {
        b_brightness = b.val >> 8;
        b_mod_brightness = mod_b.val >> 8;
    }

    // write signal indicators
    leds.setPixelColor(PRI_A_LED, a_brightness_table[a_brightness]);
    leds.setPixelColor(SEC_A_LED, a_brightness_table[a_mod_brightness]);
    leds.setPixelColor(PRI_B_LED, b_brightness_table[b_brightness]);
    leds.setPixelColor(SEC_B_LED, b_brightness_table[b_mod_brightness]);

    // write trig LEDs
    leds.setPixelColor(TRIG_A_LED, (a.eos_led)? a_colour : black);
    leds.setPixelColor(TRIG_B_LED, (b.eos_led)? b_colour : black);

    // write follow LED
    leds.setPixelColor(FLW_LED, (b.follow)? mix_colour : black);
}