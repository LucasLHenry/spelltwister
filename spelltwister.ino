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
#include "src/hardware/pins.h"
#include "src/hardware/settings.h"
#include "src/wave_algos/generator.h"
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


int8_t led_pins[8] = LED_DATA;
Adafruit_NeoPXL8 leds(NUM_LEDS, led_pins, NEO_GRB);  // led strip controller
LedRing ring(ALGO_ENC_1, ALGO_ENC_2, ALGO_BTN);  // algorithm ring controller
LedRing* _LEDRING = &ring; // used for internal ISR stuff

Waveformer A(LIN_TIME_A, MUX_A, true);  // left side controller
Waveformer B(LIN_TIME_B, MUX_B, false); // right side controller

void setup() {
    leds.begin();
    ring.begin();
    Serial.begin(9600);
}


uint64_t loop_counter = 0;  // increments per run of loop() function
void loop() {
    // read inputs
    A.read_inputs_frequent(B);
    B.read_inputs_frequent(A);

    // update values that change infrequently
    if (loop_counter % 20 == 0) {
        A.read_inputs_infrequent();
        B.read_inputs_infrequent();
    }

    ring.update(A.mod_idx_change, B.mod_idx_change);  // update the LED ring (takes cv values from A and B)
    ring.write_leds(leds);
    leds.show();

    loop_counter++;
}

repeating_timer_t timer;
bool TimerHandler(repeating_timer_t* rt);

void setup1() {
    // setup outputs!
    constexpr uint16_t max_val = (1 << BIT_DEPTH) - 1;
    constexpr uint32_t pwm_freq = CLOCK_FREQ / max_val;

    // A side pwm output setup
    gpio_set_function(PRI_OUT_A, GPIO_FUNC_PWM);
    gpio_set_function(SEC_OUT_A, GPIO_FUNC_PWM);
    uint16_t slice_num = pwm_gpio_to_slice_num(PRI_OUT_A);
    pwm_set_wrap(slice_num, max_val);
    pwm_set_enabled(slice_num, true);

    // B side pwm output setup
    gpio_set_function(PRI_OUT_B, GPIO_FUNC_PWM);
    gpio_set_function(SEC_OUT_B, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(PRI_OUT_B);
    pwm_set_wrap(slice_num, max_val);
    pwm_set_enabled(slice_num, true);

    // timer setup
    int64_t timer_period_us = - (1000 / OUTPUT_FREQ_kHz);
    alarm_pool_t* pool = alarm_pool_create(0, 1);
    alarm_pool_add_repeating_timer_us(pool, timer_period_us, TimerHandler, NULL, &timer);
}

void loop1() {}  // nothing handled here, core 1 only does the interrupt

bool TimerHandler(repeating_timer_t* rt) {
    A.update();
    B.update();
    // a.val = waveform_generator(a.acc >> 22, a.shp, a.rat, a.uslp, a.dslp);
    // b.val = waveform_generator(b.acc >> 22, b.shp, b.rat, b.uslp, b.dslp);
    // c.val = waveform_generator(c.acc >> 22, c.shp, c.rat, c.uslp, c.dslp);
    // d.val = waveform_generator(d.acc >> 22, d.shp, d.rat, d.uslp, d.dslp);

    pwm_set_gpio_level(PRI_OUT_A, 1023 - (A.generate() >> 6));
    // pwm_set_gpio_level(SEC_OUT_A, b.val >> 5);
    pwm_set_gpio_level(PRI_OUT_B, 1023 - (B.generate() >> 6));
    // pwm_set_gpio_level(SEC_OUT_B, d.val >> 5);
    return true;
}