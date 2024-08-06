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
#include "src/constants.h"
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
    rectify,
    half_freq,
    double_freq,
    invert,
    exculsive_or,
    difference,
    sum,
    frequency_mod,
    amplitude_mod,
    gate,
    shape_mod,
    ratio_mod,
    sine_pm,
    sample_rate_reduce,
    analog_pulse_pm,
    three_voice_chorus
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

bool print_flag = false;

// called when the follow button is pressed
void follow_ISR() {
    b.follow = !b.follow;
    if (b.follow) {
        b.acc = a.acc;
        b.s_acc = a.s_acc;
        b.prev_s_acc = a.prev_s_acc;
    }
    print_flag = true;
}

void setup() {
    analogReadResolution(BITS_ADC);

    // initialize objects
    a.init(&b);
    b.init(&a);
    ring.begin();
    nvm = NVMWrapper();
    leds.begin();
    // leds.setBrightness(0x9F);
    Serial.begin(9600);

    // calibration mode
    if (digitalRead(FLW_BTN) == HIGH) {
        rp2040.idleOtherCore();
        run_calibration(a, b, leds, nvm);
        rp2040.resumeOtherCore();
    }

    follow_btn.attachClick(follow_ISR);
}

void write_signal_indicator_leds();

void loop() {
    // read inputs
    a.read();
    b.read();
    follow_btn.tick();

    ring.update(a.mod_idx, b.mod_idx);
    ring.write_leds(leds);
    write_signal_indicator_leds();
    leds.show();

    if (print_flag) {
        a.print_info(true);
        b.print_info(true);
        print_flag = false;
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

void loop1() {}  // nothing handled here, core 1 only does the interrupt

bool PwmTimerHandler(repeating_timer_t* rt) {
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
    digitalWrite(TRIG_OUT_A, !a.end_of_cycle);
    digitalWrite(TRIG_OUT_B, !b.end_of_cycle);
    return true;
}

void write_signal_indicator_leds() {
    uint8_t a_brightness, a_mod_brightness, b_brightness, b_mod_brightness;
    // a is red (value, 0, 0)
    if (a.mode == ENV) {
        a_brightness = (a.val - half_y);
        a_mod_brightness = (mod_a.val - half_y);
    } else {
        a_brightness = a.val >> 8;
        a_mod_brightness = mod_a.val >> 8;
    }
    if (b.mode == ENV) {
        b_brightness = (b.val - half_y);
        b_mod_brightness = (mod_b.val - half_y);
    } else {
        b_brightness = b.val >> 8;
        b_mod_brightness = mod_b.val >> 8;
    }

    leds.setPixelColor(PRI_A_LED, a_brightness_table[a_brightness]);
    leds.setPixelColor(SEC_A_LED, a_brightness_table[a_mod_brightness]);
    leds.setPixelColor(PRI_A_LED, b_brightness_table[b_brightness]);
    leds.setPixelColor(SEC_A_LED, b_brightness_table[b_mod_brightness]);

    // write trig LEDs
    leds.setPixelColor(TRIG_A_LED, (a.eos_led)? a_colour : black);
    leds.setPixelColor(TRIG_B_LED, (b.eos_led)? b_colour : black);

    // write follow LED
    leds.setPixelColor(FLW_LED, (b.follow)? mix_colour : black);
}