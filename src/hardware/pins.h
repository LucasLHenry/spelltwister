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
// not pins
#define NUM_LEDS 23
#define NUM_RING_LEDS 16
#define FLW_LED 16
#define TRIG_B_LED 17
#define PRI_B_LED 18
#define SEC_B_LED 19
#define SEC_A_LED 20
#define PRI_A_LED 21
#define TRIG_A_LED 22



#define ALGO_ENC_1 23
#define ALGO_ENC_2 22
#define ALGO_BTN 24
#define FLW_BTN 7

#define MUX_S0 19
#define MUX_S1 20
#define MUX_S2 21

// mux A assignments
#define R_CV_A   3
#define R_POT_A  0
#define S_CV_A   2
#define S_POT_A  1
#define M_CV_A   4
#define SW_1_A   5
#define SW_2_A   6
#define EXP_CV_A 7

// mux B assignments
#define R_CV_B   1
#define R_POT_B  2
#define S_CV_B   0
#define S_POT_B  3
#define M_CV_B   4
#define SW_1_B   5
#define SW_2_B   6
#define EXP_CV_B 7

#endif