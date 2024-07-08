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

#define CONFIGDATA_NUM_FIELDS 6

class NVMWrapper {
    uint32_t _size;
    public:
        NVMWrapper();

};

#endif