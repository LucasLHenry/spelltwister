#include "nvm_wrapper.h"

bool config_data_eq(ConfigData l, ConfigData r) {
    if (l.vo_offset != r.vo_offset)           return false;
    if (l.vo_scale != r.vo_scale)             return false;
    if (l.fm_offset != r.fm_offset)           return false;
    if (l.mod_offset != r.mod_offset)         return false;
    if (l.shp_pot_offset != r.shp_pot_offset) return false;
    if (l.shp_cv_offset != r.shp_cv_offset)   return false;
    if (l.rat_pot_offset != r.rat_pot_offset) return false;
    if (l.rat_cv_offset != r.rat_cv_offset)   return false;
    return true;
}

void print_config_data(ConfigData& conf) {
    Serial.print("vo_offset: ");
    Serial.println(conf.vo_offset);
    Serial.print("vo_scale: ");
    Serial.println(conf.vo_scale);
    Serial.print("fm_offset: ");
    Serial.println(conf.fm_offset);
    Serial.print("mod_offset: ");
    Serial.println(conf.mod_offset);
    Serial.print("shp_pot_offset: ");
    Serial.println(conf.shp_pot_offset);
    Serial.print("shp_cv_offset: ");
    Serial.println(conf.shp_cv_offset);
    Serial.print("rat_pot_offset: ");
    Serial.println(conf.rat_pot_offset);
    Serial.print("rat_cv_offset: ");
    Serial.println(conf.rat_cv_offset);
}

NVMWrapper::NVMWrapper() {
    EEPROM.begin(sizeof(MemoryLayout));
    mem = EEPROM.get(0, mem);
    data_in_eeprom = (mem.data_exists == 'Y');
}

ConfigData NVMWrapper::get_config_data(bool is_a) {
    if (!data_in_eeprom) {
        return (is_a)? a_default_config_data : b_default_config_data;
    }
    return (is_a)? mem.a_data : mem.b_data;
}

void NVMWrapper::set_config_data(bool is_a, ConfigData& conf) {
    if (is_a) {
        mem.a_data = conf;
    } else {
        mem.b_data = conf;
    }
}

void NVMWrapper::save_data() {
    mem.data_exists = 'Y';
    EEPROM.put(0, mem);
    EEPROM.commit();
}