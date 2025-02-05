#include "waveformer.h"

// from the mux wiring, different for the two sides
uint16_t A_mux_sigs[] = {R_CV_A, R_POT_A, S_CV_A, S_POT_A, M_CV_A, SW_1_A, SW_2_A, EXP_CV_A};
uint16_t B_mux_sigs[] = {R_CV_B, R_POT_B, S_CV_B, S_POT_B, M_CV_B, SW_1_B, SW_2_B, EXP_CV_B};

Waveformer::Waveformer(bool is_A, int mux_pin, int time_pin):
    is_a {is_A},
    mux(admux::Pin(mux_pin, INPUT, admux::PinType::Analog), admux::Pinset(MUX_S0, MUX_S1, MUX_S2)),
    lin_time_pin(time_pin),
    pitch_filter(30, 100, false),
    rat_filter  (45, 200, false),
    shp_filter  (45, 200, false),
    mod_filter  (45, 200, false),
    core(acc_downshift)
{
    if (is_a) {
        mux_sigs = A_mux_sigs;
        configs = a_default_config_data;
    } else {
        mux_sigs = B_mux_sigs;
        configs = b_default_config_data;
    }
}

void Waveformer::init(Waveformer* other) {
    core.pha = 200 * hz_phasor;
    rat = 1023;
    shp = 511;
    uslp = calc_upslope(rat);
    dslp = calc_downslope(rat);
    mode = VCO;
    running = true;
    _other = other;
}

void Waveformer::update() {
    prev_eos = end_of_cycle;
    core.update();

    if (core.overflow && running) {
        end_of_cycle = true;
        eos_led = true;
        EOS_start_time = update_counter;
        if (mode == ENV) running = false;
    }

    if (update_counter == EOS_start_time + trig_length_in_updates) end_of_cycle = false;
    if (update_counter == EOS_start_time + trig_led_length_in_updates) eos_led = false;

    update_counter++;
}

void Waveformer::generate() {
    val = (running)? waveform_generator(core.s_acc, shp, rat, uslp, dslp) : 0;
    if (mode == ENV) val = (val >> 1) + half_y;
    if (core.s_acc < rat && running) acc_by_val[val >> bit_diff] = core.acc;  // for retriggering of envelopes
    if (running && val < 64000) val += rand_u32() >> 28;  // amplitude dithering
}

void Waveformer::read() {
    read_all();
    oversample_pitch();
    uint16_t rat_buf = calc_ratio();
    if (rat_buf != rat) {
        rat = rat_buf;
        uslp = calc_upslope(rat);
        dslp = calc_downslope(rat);
    }

    shp = calc_shape();
    core.pha = calc_phasor();
    mode = get_mode();
    mod_idx = calc_mod_idx();
    if (mode != ENV) running = true;
    if (follow_mode != DISABLED) set_follow_mode();
}

uint16_t Waveformer::calc_shape() {
    int32_t calibrated_pot = configs.shp_pot_offset - static_cast<int16_t>(raw_vals.shape_pot);
    int16_t calibrated_cv = raw_vals.shape_cv - static_cast<int16_t>(configs.shp_cv_offset);
    calibrated_cv += calibrated_pot;
    if (calibrated_cv < 0) calibrated_cv = 0;
    if (calibrated_cv > max_adc) calibrated_cv = max_adc;
    return shp_filter.get_next(calibrated_cv) >> 1;
}

uint16_t Waveformer::calc_ratio() {
    int32_t calibrated_pot = configs.rat_pot_offset - static_cast<int16_t>(raw_vals.ratio_pot);
    int16_t calibrated_cv = raw_vals.ratio_cv - static_cast<int16_t>(configs.rat_cv_offset);
    calibrated_cv += calibrated_pot;
    if (calibrated_cv < 0) calibrated_cv = 0;
    if (calibrated_cv > max_adc) calibrated_cv = max_adc;
    return rat_filter.get_next(calibrated_cv) >> 1;
}

// follow behaviour table:
//         B VCO               B LFO               B ENV
//       -----------------------------------------------
// A VCO | pitch intervals     same v/o            same v/o     
//       |
// A LFO | same v/o            clock divider       same start time
//       |
// A ENV | same v/o            same start time     same start time

// format is {multiply by, divide by}
uint16_t vco_follow_intervals[8][2] = {
    {1, 2},     // octave down
    {1, 1},     // unity
    {6, 5},     // minor third
    {5, 4},     // major third
    {4, 3},     // perfect fourth
    {3, 2},     // perfect fifth
    {9, 5},     // minor seventh
    {2, 1},     // octave
};

uint16_t lfo_follow_intervals[8][2] = {
    {1,  8},
    {1,  4}, 
    {1,  2},
    {1,  1}, 
    {2,  1},
    {4,  1}, 
    {8,  1},
    {16, 1},
};

uint32_t Waveformer::calc_phasor() {
    int16_t calibrated_lin = (raw_vals.fm - configs.fm_offset) << 1;
    int16_t filtered_val = pitch_filter.get_next(raw_vals.pitch);

    int32_t calibrated_exp = ((static_cast<int16_t>(configs.vo_offset) - static_cast<int16_t>(filtered_val)) * configs.vo_scale) >> 8;
    calibrated_exp += calibrated_lin;
    phasor_idx = (calibrated_exp < 0)? 0 : ((calibrated_exp > phasor_arr_len)? phasor_arr_len : calibrated_exp); // CLIP(calibrated_exp, 0, phasor_arr_len);

    if (!is_a && follow_mode != DISABLED) {
        if (follow_mode == TONE_INTERVALS) {
            uint32_t mult_div = get_follow_mult_div();
            uint16_t mult = mult_div >> 16;
            uint16_t div = mult_div & 0xFFFF;

            uint32_t follow_phasor = (_other->core.pha * mult) / div;
            if (mult == 1 && div == 1) follow_phasor = static_cast<uint32_t>(follow_phasor * 1.002);
            if (follow_phasor > max_pha) return max_pha;
            else return follow_phasor;
        } else if (follow_mode == CLOCK_DIV_MULT) {
            uint32_t mult_div = get_follow_mult_div();
            uint16_t mult = mult_div >> 16;
            uint16_t div = mult_div & 0xFFFF;

            if (prev_follow_mode != CLOCK_DIV_MULT || follow_interval_idx_changed) {
                // reset phase
                core.acc = static_cast<uint32_t>(((static_cast<uint64_t>(_other->core.acc) * mult) / div) % max_acc);
            }

            uint32_t follow_phasor = (_other->core.pha * mult) / div;
            if (follow_phasor > max_pha) return max_pha;
            else return follow_phasor;
        } else if (follow_mode == SAME_VOLT_PER_OCT) {
            phasor_idx = _other->phasor_idx;
        } else if (follow_mode == SYNCED_START) {
            ;  // logic for this happens in a_sync_ISR, included for completeness
        }
    }

    if (mode == VCO) return phasor_table[phasor_idx];
    else return slow_phasor_table[phasor_idx];
}

Mode Waveformer::get_mode() {
    bool val1 = (mux.read(mux_sigs[SW_1_IDX]) > half_adc)? true : false;
    bool val2 = (mux.read(mux_sigs[SW_2_IDX]) > half_adc)? true : false;

    // different wiring for a vs b side
    if (is_a) {
        if (val1 && !val2) return ENV;
        else if (!val1 && val2) return VCO;
        else return LFO;
    } else {
        if (val1 && !val2) return VCO;
        else if (!val1 && val2) return ENV;
        else return LFO;
    }
}

int8_t Waveformer::calc_mod_idx() {
    int16_t _cv = mod_filter.get_next(raw_vals.algo_mod) >> 7;
    int16_t _offset = configs.mod_offset >> 7;
    int16_t calibrated_cv = static_cast<int16_t>((_offset - _cv) * 8 / 9);
    return static_cast<int8_t>(calibrated_cv);
}

void Waveformer::reset() {
    if (mode == ENV && core.s_acc >= rat && running && rat > 100) {
        core.reset(acc_by_val[val >> 5]);
    } else {
        core.reset(0);
    }
}

void Waveformer::read_all() {
    raw_vals.pitch     = mux.read(mux_sigs[VO_IDX]);   // pitch
    raw_vals.fm        = analogRead(lin_time_pin);     // fm
    raw_vals.shape_pot = mux.read(mux_sigs[S_PT_IDX]); // shape pot
    raw_vals.shape_cv  = mux.read(mux_sigs[S_CV_IDX]); // shape cv
    raw_vals.ratio_pot = mux.read(mux_sigs[R_PT_IDX]); // ratio pot
    raw_vals.ratio_cv  = mux.read(mux_sigs[R_CV_IDX]); // ratio cv
    raw_vals.algo_mod  = mux.read(mux_sigs[M_CV_IDX]); // algo mod
}

void Waveformer::oversample_pitch() {
    uint64_t total = raw_vals.pitch;
    for (int i = 0; i < 7; i++) {
        total += mux.read(mux_sigs[VO_IDX]);
    }
    raw_vals.pitch = static_cast<uint16_t>(total / 8);
}

AllInputs Waveformer::get_all(uint16_t repeats) {
    if (repeats == 1) {
        read_all();
        return raw_vals;
    }
    uint64_t all_vals_sums[7];
    for (int i = 0; i < repeats; i++) {
        read_all();
        all_vals_sums[0] += raw_vals.pitch;    
        all_vals_sums[1] += raw_vals.fm;
        all_vals_sums[2] += raw_vals.shape_pot;
        all_vals_sums[3] += raw_vals.shape_cv;
        all_vals_sums[4] += raw_vals.ratio_pot;
        all_vals_sums[5] += raw_vals.ratio_cv;
        all_vals_sums[6] += raw_vals.algo_mod;
    }
    
    AllInputs reads = {
        static_cast<uint16_t>(all_vals_sums[0] / repeats),
        static_cast<uint16_t>(all_vals_sums[1] / repeats),
        static_cast<uint16_t>(all_vals_sums[2] / repeats),
        static_cast<uint16_t>(all_vals_sums[3] / repeats),
        static_cast<uint16_t>(all_vals_sums[4] / repeats),
        static_cast<uint16_t>(all_vals_sums[5] / repeats),
        static_cast<uint16_t>(all_vals_sums[6] / repeats)
    };

    return reads;
}

void Waveformer::toggle_follow_mode() {
    if (follow_mode != DISABLED) {
        follow_mode = DISABLED;
    } else {
        set_follow_mode();
    }
}

bool Waveformer::get_follow() {
    return follow_mode != DISABLED;
}

void Waveformer::set_follow_mode() {
    prev_follow_mode = follow_mode;
    if (mode == VCO && _other->mode == VCO) follow_mode = TONE_INTERVALS;
    else if (mode == LFO && _other->mode == LFO) follow_mode = CLOCK_DIV_MULT;
    else if (mode == VCO || _other->mode == VCO) follow_mode = SAME_VOLT_PER_OCT;
    else follow_mode = SYNCED_START;
}

uint32_t Waveformer::get_follow_mult_div() {
    uint16_t new_interval_idx = static_cast<uint16_t>((static_cast<uint32_t>(phasor_idx)*6) >> 11);
    follow_interval_idx_changed = follow_interval_idx != new_interval_idx;
    follow_interval_idx = new_interval_idx;
    uint16_t multiplier, divider;
    if (follow_mode == TONE_INTERVALS) {
        multiplier = vco_follow_intervals[new_interval_idx][0];
        divider = vco_follow_intervals[new_interval_idx][1];
    } else if (follow_mode == CLOCK_DIV_MULT) {
        multiplier = lfo_follow_intervals[new_interval_idx][0];
        divider = lfo_follow_intervals[new_interval_idx][1];
    } else {
        multiplier = 1;
        divider = 1;
    }
    return (static_cast<uint32_t>(multiplier) << 16) | divider;
}

void Waveformer::print_info(bool verbose) {
    Serial.println((is_a)? "A" : "B");

    Serial.print("mode: ");
    if (mode == ENV) Serial.println("ENV");
    else if (mode == VCO) Serial.println("VCO");
    else Serial.println("LFO");

    if (verbose) {
        Serial.println("RAW VALS");
        Serial.print("mod val: ");
        Serial.println(raw_vals.algo_mod);

        Serial.print("rat pot: ");
        Serial.println(raw_vals.ratio_pot);

        Serial.print("rat cv: ");
        Serial.println(raw_vals.ratio_cv);

        Serial.print("shp pot: ");
        Serial.println(raw_vals.shape_pot);

        Serial.print("shp cv: ");
        Serial.println(raw_vals.shape_cv);

        Serial.print("pitch: ");
        Serial.println(raw_vals.pitch);

        Serial.print("fm: ");
        Serial.println(raw_vals.fm);

        Serial.println("PARAMS");
        Serial.print("ratio: ");
        Serial.println(rat);

        Serial.print("shape: ");
        Serial.println(shp);

        Serial.print("phasor: ");
        Serial.println(core.pha);
    }
}

uint32_t rand_u32() {
    static uint64_t val = 1234;  // seed
    val = (val*134775813 + 1) % (1 << 31);
    return static_cast<uint32_t>(val);
}