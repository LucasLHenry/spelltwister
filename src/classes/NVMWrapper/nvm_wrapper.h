#include <Arduino.h>
#include <EEPROM.h>

#ifndef NVM_WRAPPER_H
#define NVM_WRAPPER_H

typedef struct ConfigData {
    uint16_t vo_offset;
    uint16_t vo_scale;
    uint16_t fm_offset;
    uint16_t mod_offset;
    uint16_t shape_offset;
    uint16_t ratio_offset;
} ConfigData;

ConfigData a_default_config_data = {264, 322, 570, 567, 570, 570};
ConfigData b_default_config_data = {264, 322, 570, 570, 570, 574};

bool config_data_eq(ConfigData l, ConfigData r);

typedef struct MemoryLayout {
    bool data_exists;
    ConfigData a_data;
    ConfigData b_data;
} MemoryLayout;

MemoryLayout* _MEM;

class NVMWrapper {
    MemoryLayout mem;
    char data_in_eeprom;
    public:
        NVMWrapper();
        ConfigData get_config_data(bool is_a);
        ConfigData write_config_data(bool is_a, ConfigData* conf);
        void save_data();
};

#endif