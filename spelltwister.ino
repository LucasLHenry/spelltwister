#include <Arduino.h>

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

void setup() {
    // initialize objects
}

void loop() {
    // read inputs
}

void setup1() {
    // initialize output sample generation timer
    // initialize PWM outputs
    // attach sync/trig interrupts
}

void loop1() {
    // nothing to do here
}