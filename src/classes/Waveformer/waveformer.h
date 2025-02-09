#include <Arduino.h>
#include <Mux.h>

#include "../../wave_algos/generator.h"
#include "../../hardware/pins.h"
#include "../../constants.hpp"
#include "../../tables/phasors.h"
#include "../NVMWrapper/nvm_wrapper.h"
#include "../../hardware/adc_filter.h"
#include "../tables/adc_linearization_table.h"

/*
This is the core signal generator class, there is one for the left side and one for the right (A and B). I called it a waveformer
because it's not technically an oscillator (it does envelopes). There are four main controls: time, shape, ratio, and mode. The
first three are continuous, with knobs and cv inputs (time has two cv inputs, FM (with an attenuator) and V/O). Mode is a 3 way
switch, which goes between VCO, LFO, and ENV. The time control goes from 0 to 4095, while the shape and ratio controls goes from
0 to 2047.
*/

#ifndef WAVEFORMER_H
#define WAVEFORMER_H

// ms * 1s/1000ms * 44100 updates/s
constexpr uint64_t trig_length_in_updates = static_cast<uint64_t>(TRIG_LENGTH_MS * 44.1);
constexpr uint64_t trig_led_length_in_updates = static_cast<uint64_t>(TRIG_LED_LENGTH_MS * 44.1);

enum Mode {VCO, LFO, ENV};

enum FollowBehaviour {
    DISABLED,
    TONE_INTERVALS,
    CLOCK_DIV_MULT,
    SAME_VOLT_PER_OCT,
    SYNCED_START,
};

typedef struct AllInputs {
    uint16_t pitch;
    uint16_t fm;
    uint16_t shape_pot;
    uint16_t shape_cv;
    uint16_t ratio_pot;
    uint16_t ratio_cv;
    uint16_t algo_mod;
} AllInputs;

uint32_t rand_u32();

// DDS stands for Direct Digital Synthesis
class DDS_Wrapper {
    uint16_t _shift_amt;
    public:
    uint32_t acc, pha;
    uint16_t s_acc, prev_s_acc, clean_s_acc_;
    bool overflow;
    DDS_Wrapper(uint16_t shift_amt): _shift_amt{shift_amt} {}

    void update() {
        prev_s_acc = clean_s_acc_;
        acc += pha;
        uint32_t dither_val_ = rand_u32() >> 12;
        clean_s_acc_ = acc >> _shift_amt;
        overflow = (prev_s_acc > clean_s_acc_);
        s_acc = (acc + dither_val_) >> _shift_amt;
    }

    void reset(uint32_t new_acc) {
        acc = new_acc;
        s_acc = acc >> _shift_amt;
        prev_s_acc = s_acc;
        clean_s_acc_ = s_acc;
    }

    void set(DDS_Wrapper& other) {
        acc = other.acc;
        s_acc = other.s_acc;
        prev_s_acc = other.prev_s_acc;
    }
};

class Waveformer {
    uint16_t raw_mod;
    int lin_time_pin, mux_pin;
    ADC_Filter pitch_filter, rat_filter, shp_filter, mod_filter;
    uint16_t* mux_sigs;
    admux::Mux mux;
    uint16_t calc_ratio();
    uint16_t calc_shape();
    uint32_t calc_phasor();
    int8_t calc_mod_idx();
    Mode get_mode();
    uint32_t acc_by_val[2048];  // for envelope retriggering
    Waveformer* _other;
    uint64_t update_counter, EOS_start_time;
    AllInputs raw_vals;
    FollowBehaviour prev_follow_mode;
    uint32_t get_follow_mult_div();
    void oversample_pitch();
    uint16_t follow_interval_idx;
    bool follow_interval_idx_changed;
    public:
        FollowBehaviour follow_mode;
        DDS_Wrapper core;
        ConfigData configs;
        bool is_a;
        uint16_t uslp, dslp;
        Mode mode;
        bool running;
        uint16_t rat, shp;
        uint16_t val;
        Waveformer(bool is_A, int mux_pin, int time_pin);
        void init(Waveformer* other);
        void update();
        void generate();
        void read();
        void reset();
        void read_all();
        bool get_follow();
        AllInputs get_all(uint16_t repeats);
        void print_info(bool verbose);
        bool end_of_cycle, prev_eos;
        bool eos_led;
        int8_t mod_idx;
        uint16_t phasor_idx;
        void toggle_follow_mode();
        void set_follow_mode();
};

// order for mux_assignemnts is ratio cv, ratio pot, shape cv, shape pot, algo cv, switch 1, switch 2, exp time cv
#define R_CV_IDX 0
#define R_PT_IDX 1
#define S_CV_IDX 2
#define S_PT_IDX 3
#define M_CV_IDX 4
#define SW_1_IDX 5
#define SW_2_IDX 6
#define VO_IDX   7

#endif