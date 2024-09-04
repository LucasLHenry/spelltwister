#include "waveformer.h"

// from the mux wiring, different for the two sides
uint16_t A_mux_sigs[] = {R_CV_A, R_POT_A, S_CV_A, S_POT_A, M_CV_A, SW_1_A, SW_2_A, EXP_CV_A};
uint16_t B_mux_sigs[] = {R_CV_B, R_POT_B, S_CV_B, S_POT_B, M_CV_B, SW_1_B, SW_2_B, EXP_CV_B};

Waveformer::Waveformer(bool is_A, int mux_pin, int time_pin):
    is_a {is_A},
    mux(admux::Pin(mux_pin, INPUT, admux::PinType::Analog), admux::Pinset(MUX_S0, MUX_S1, MUX_S2)),
    lin_time_pin(time_pin),
    algo_read(0, true),
    pitch_filter(30, 200, is_A),
    rat_filter  (45, 200, false),
    shp_filter  (45, 200, false),
    algo_filter (45, 200, false),
    core(21) // acc is 32b, 32-21 = 11b → 0-2047 range
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
    core.pha = 200 * HZPHASOR;
    rat = 1023;
    shp = 511;
    uslp = calc_upslope(1023);
    dslp = calc_downslope(1023);
    algo_read.setAnalogResolution(max_adc + 1);
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
}

void Waveformer::read() {
    read_all();
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
}

uint16_t Waveformer::calc_shape() {
    int32_t calibrated_pot = configs.shp_pot_offset - raw_vals.shape_pot;
    int16_t calibrated_cv = configs.shp_cv_offset - raw_vals.shape_cv;
    return shp_filter.get_next(CLIP(calibrated_pot + calibrated_cv, 0, max_adc)) >> 1;
}

uint16_t Waveformer::calc_ratio() {
    int32_t calibrated_pot = configs.rat_pot_offset - raw_vals.ratio_pot;
    int16_t calibrated_cv = configs.rat_cv_offset - raw_vals.ratio_cv;
    return rat_filter.get_next(CLIP(calibrated_pot + calibrated_cv, 0, max_adc)) >> 1;
}

uint32_t Waveformer::calc_phasor() {
    if (!is_a && follow) return _other->core.pha;
    int16_t calibrated_lin = (raw_vals.fm - configs.fm_offset) / FM_ATTENUATION;
    uint16_t filtered_val = pitch_filter.get_next(raw_vals.pitch);

    int32_t calibrated_exp = configs.vo_offset - ((filtered_val * configs.vo_scale) >> 8);
    uint16_t processed_val = CLIP(calibrated_exp + calibrated_lin, 0, max_adc);

    if (mode == VCO) return phasor_table[processed_val];
    else return slow_phasor_table[processed_val];
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
    // uint16_t _cv = algo_filter.get_next(raw_vals.algo_mod) >> 8;
    algo_read.update(raw_vals.algo_mod);
    uint16_t _cv = algo_read.getValue() >> 8;
    uint16_t _offset = configs.mod_offset >> 8;
    int16_t calibrated_cv = static_cast<int16_t>(_cv) - static_cast<int16_t>(_offset);
    return static_cast<int8_t>(-calibrated_cv);
}

void Waveformer::reset() {
    if (mode == ENV && core.s_acc >= rat) {
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

AllInputs Waveformer::get_all(uint16_t repeats) {
    if (repeats == 1) {
        read_all();
        return raw_vals;
    }
    uint16_t all_vals_sums[7];
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