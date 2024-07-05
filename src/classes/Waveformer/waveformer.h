#include <Arduino.h>
#include <Mux.h>
#include <ResponsiveAnalogRead.h>

#include "../../wave_algos/generator.h"
#include "../../hardware/pins.h"
#include "../../constants.h"
#include "../../tables/phasors.h"
#include "../../tables/slow_phasors.h"

#ifndef WAVEFORMER_H
#define WAVEFORMER_H

// ms * 1s/1000ms * 44100 updates/s
constexpr uint64_t trig_length_in_updates = static_cast<uint64_t>(TRIG_LENGTH_MS * 44.1);
constexpr uint64_t trig_led_length_in_updates = static_cast<uint64_t>(TRIG_LED_LENGTH_MS * 44.1);

typedef struct ConfigData {
    uint16_t vo_offset;
    uint16_t vo_scale;
    uint16_t fm_offset;
    uint16_t mod_offset;
    uint16_t shape_offset;
    uint16_t ratio_offset;
} ConfigData;

enum Mode {VCO, LFO, ENV};

class Waveformer {
    uint16_t raw_mod;
    int lin_time_pin, mux_pin;
    ResponsiveAnalogRead rat_read, shp_read, time_read, algo_read;
    uint16_t* mux_sigs;
    admux::Mux mux;
    uint16_t get_ratio();
    uint16_t get_shape();
    uint32_t get_phasor();
    int8_t get_mod_idx_change();
    Mode get_mode();
    ConfigData configs;
    uint32_t acc_by_val[2048];  // for envelope retriggering
    Waveformer* _other;
    int8_t mod_idx, prev_mod_idx;
    uint64_t update_counter, EOS_start_time;
    public:
        bool is_a;
        uint16_t uslp, dslp;
        bool follow;
        Mode mode;
        bool running;
        uint16_t rat, shp;
        uint32_t acc, pha;
        uint16_t s_acc, prev_s_acc;
        uint16_t val;
        Waveformer(bool is_A, int mux_pin, int time_pin);
        void init(Waveformer* other);
        void update();
        void generate();
        void read();
        void reset();
        bool end_of_cycle, prev_eos;
        bool eos_led;
        int8_t mod_idx_change;
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