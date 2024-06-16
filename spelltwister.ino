/*
Developed by Lucas Henry, May 2024

This file is meant to be as readable as possible, showing only high-level logic. All of the meat of this
project is within the src directory, so dig into that to do your hacking.
*/

#include <Arduino.h>
#include <Adafruit_NeoPXL8.h>

#include "src/classes/Waveformer/waveformer.h"
#include "src/classes/LedRing/led_ring.h"
#include "src/hardware/pins.h"

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

Waveformer a(0, 1);
Waveformer b(2, 3);

void setup() {
    // initialize objects
    // a.init();
    // b.init();
    // a.pha = 0.1 * HZPHASOR;
    // b.pha = 0.9 * HZPHASOR;

    // a.rat = b.rat = 511;
    // a.shp = b.shp = 511;
    // a.uslp = b.uslp = calc_upslope(511);
    // a.dslp = b.dslp = calc_downslope(511);

    leds.begin();
    ring.begin();
    Serial.begin(9600);
}

void loop() {
    // read inputs
    ring.update(0, 0);
    ring.write_leds(leds);
    leds.show();
    Serial.println(ring.a_idx);
}

void setup1() {
    // initialize output sample generation timer
    // initialize PWM outputs
    // attach sync/trig interrupts
}

void loop1() {
    // nothing to do here
}