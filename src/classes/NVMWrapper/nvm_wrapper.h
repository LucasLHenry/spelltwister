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

const ConfigData a_default_config_data = {4759, 300, 2261, 2269, 4081, 4082, 2280, 2281};
const ConfigData b_default_config_data = {4774, 300, 2268, 2281, 4081, 4080, 2284, 2272};

bool config_data_eq(ConfigData l, ConfigData r);

typedef struct MemoryLayout {
    bool data_exists;
    ConfigData a_data;
    ConfigData b_data;
} MemoryLayout;

class NVMWrapper {
    MemoryLayout mem;
    char data_in_eeprom;
    public:
        NVMWrapper();
        ConfigData get_config_data(bool is_a);
        void write_config_data(bool is_a, ConfigData& conf);
        void save_data();
};

#endif