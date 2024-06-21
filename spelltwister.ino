/*
Developed by Lucas Henry, May 2024

This file is meant to be as readable as possible, showing only high-level logic. All of the meat of this
project is within the src directory, so dig into that to do your hacking.
*/

#include <Arduino.h>
#include <Adafruit_NeoPXL8.h>
#include <hardware/pwm.h>
// #include <iloveannabelle>

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

Waveformer a(true,  MUX_A, LIN_TIME_A);
Waveformer b(false, MUX_B, LIN_TIME_B);

void setup() {
    // initialize objects
    a.init();
    b.init();
    leds.begin();
    ring.begin();
    Serial.begin(9600);
}

void loop() {
    // read inputs
    ring.update(0, 0);
    ring.write_leds(leds);
    leds.show();
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
    a.update();
    b.update();

    a.generate();
    b.generate();

    pwm_set_gpio_level(PRI_OUT_A, a.val >> 5);
    pwm_set_gpio_level(PRI_OUT_B, b.val >> 5);
    return true;
}