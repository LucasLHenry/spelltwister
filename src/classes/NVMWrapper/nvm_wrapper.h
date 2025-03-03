#include <Arduino.h>
#include <EEPROM.h>

#ifndef NVM_WRAPPER_H
#define NVM_WRAPPER_H

#define NUM_SCALE_VOLTAGES 5

typedef struct ConfigData {
    // values corresponding to the voltages {0,1,2,...,NUM_SCALE_VOLTAGES}
    // used to decide which offset and scale to use
    uint16_t vo_margins[NUM_SCALE_VOLTAGES+1];
    // if x is between xi and x{i+1}, where xis are from vo_margins,
    // the output value is (x*vo_scale[i] + vo_offset[i]) >> vo_oversample_amt
    int32_t vo_offset[NUM_SCALE_VOLTAGES];
    int32_t vo_scale[NUM_SCALE_VOLTAGES];
    int16_t fm_offset;
    int16_t mod_offset;
    int16_t shp_pot_offset;
    int16_t rat_pot_offset;
    int16_t shp_cv_offset;
    int16_t rat_cv_offset;
    int16_t pri_out_offset;
    int16_t sec_out_offset;
} ConfigData;

const ConfigData a_default_config_data = {3264, 260, 2272, 2271, 4085, 4083, 2291, 2275};
const ConfigData b_default_config_data = {3274, 260, 2275, 2255, 4082, 4083, 2283, 2264};

bool config_data_eq(ConfigData l, ConfigData r);

void print_config_data(ConfigData& conf);

typedef struct {
    char data_exists;
    bool encoder_reversed;
    ConfigData a_data;
    ConfigData b_data;
} ConfigMemoryLayout;

typedef struct {
    char data_exists;
    uint8_t a_idx;
    uint8_t b_idx;
    bool follow_on;
} ModPosMemoryLayout;

class NVMWrapper {
    ConfigMemoryLayout conf_mem;
    ModPosMemoryLayout mod_mem;
    uint8_t saved_a, saved_b;
    bool saved_follow;
    public:
        bool conf_data_stored;
        bool mod_data_stored;
        NVMWrapper();
        ConfigData get_config_data(bool is_a);
        void set_config_data(bool is_a, ConfigData& conf);
        void set_encoder_direction(bool reversed);
        bool get_encoder_direction();
        void save_config_data();
        void set_follow(bool follow);
        bool get_follow();

        uint8_t get_mod_pos(bool is_a);
        void set_mod_pos(bool is_a, uint8_t mod_pos);
        void save_mod_pos();
}; 

#endif