#include <Arduino.h>
#include <EEPROM.h>

#ifndef NVM_WRAPPER_H
#define NVM_WRAPPER_H

typedef struct ConfigData {
    uint16_t vo_offset;
    uint16_t vo_scale;
    uint16_t fm_offset;
    uint16_t mod_offset;
    uint16_t shp_pot_offset;
    uint16_t rat_pot_offset;
    uint16_t shp_cv_offset;
    uint16_t rat_cv_offset;
} ConfigData;

const ConfigData a_default_config_data = {3264, 260, 2272, 2271, 4085, 4083, 2291, 2275};
const ConfigData b_default_config_data = {3274, 260, 2275, 2255, 4082, 4083, 2283, 2264};

bool config_data_eq(ConfigData l, ConfigData r);

void print_config_data(ConfigData& conf);

typedef struct {
    char data_exists;
    ConfigData a_data;
    ConfigData b_data;
} ConfigMemoryLayout;

typedef struct {
    char data_exists;
    uint8_t a_idx;
    uint8_t b_idx;
} ModPosMemoryLayout;

class NVMWrapper {
    ConfigMemoryLayout conf_mem;
    ModPosMemoryLayout mod_mem;
    uint8_t saved_a, saved_b;
    public:
        bool conf_data_stored;
        bool mod_data_stored;
        NVMWrapper();
        ConfigData get_config_data(bool is_a);
        void set_config_data(bool is_a, ConfigData& conf);
        void save_config_data();

        uint8_t get_mod_pos(bool is_a);
        void set_mod_pos(bool is_a, uint8_t mod_pos);
        void save_mod_pos();
}; 

#endif