// pin assignments for the SAM D21
// use these names to refer to them
#ifndef PINS_H
#define PINS_H


#define LIN_TIME_A 27
#define SIG_IN_A 17
#define MUX_A 26
#define PRI_OUT_A 13  // don't know what PWM reg this corresponds to
#define SEC_OUT_A 12  // same as above
#define TRIG_OUT_A 8

#define LIN_TIME_B 28
#define SIG_IN_B 18
#define MUX_B 29
#define PRI_OUT_B 1  // don't know what PWM reg this corresponds to
#define SEC_OUT_B 0  // same as above
#define TRIG_OUT_B 5

#define LED_DATA {25, -1, -1, -1, -1, -1, -1, -1}

#define ALGO_ENC_1 23
#define ALGO_ENC_2 22
#define ALGO_BTN 24
#define FLW_BTN 7

#define MUX_S0 19
#define MUX_S1 20
#define MUX_S2 21

#endif