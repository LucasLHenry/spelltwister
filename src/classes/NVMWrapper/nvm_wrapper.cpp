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
    Serial.println("vo_offset: ");
    for (uint16_t i = 0; i < NUM_SCALE_VOLTAGES; i++) {
        Serial.println(conf.vo_offset[i]);
    }
    Serial.println("vo_scale: ");
    for (uint16_t i = 0; i < NUM_SCALE_VOLTAGES; i++) {
        Serial.println(conf.vo_scale[i]);
    }
    Serial.println("vo margins: ");
    for (uint16_t i = 0; i < NUM_SCALE_VOLTAGES+1; i++) {
        Serial.println(conf.vo_margins[i]);
    }
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
    Serial.print("pri_out_offset: ");
    Serial.println(conf.pri_out_offset);
    Serial.print("sec_out_offset: ");
    Serial.println(conf.sec_out_offset);
}

NVMWrapper::NVMWrapper() {
    EEPROM.begin(sizeof(ConfigMemoryLayout) + sizeof(ModPosMemoryLayout) + 1);
    conf_mem = EEPROM.get(0, conf_mem);
    mod_mem = EEPROM.get(sizeof(ConfigMemoryLayout) + 1, mod_mem);
    conf_data_stored = (conf_mem.data_exists == 'Y');
    mod_data_stored = (mod_mem.data_exists == 'Y');
    saved_a = mod_mem.a_idx;
    saved_b = mod_mem.b_idx;
}

ConfigData NVMWrapper::get_config_data(bool is_a) {
    if (!conf_data_stored) return (is_a)? a_default_config_data : b_default_config_data;
    return (is_a)? conf_mem.a_data : conf_mem.b_data;
}

void NVMWrapper::set_config_data(bool is_a, ConfigData& conf) {
    if (is_a) conf_mem.a_data = conf;
    else conf_mem.b_data = conf;
}

void NVMWrapper::save_config_data() {
    conf_mem.data_exists = 'Y';
    EEPROM.put(0, conf_mem);
    EEPROM.commit();
}

void NVMWrapper::set_encoder_direction(bool reversed) {
    conf_mem.encoder_reversed = reversed;
}

bool NVMWrapper::get_encoder_direction() {
    if (!conf_data_stored) return false;
    return conf_mem.encoder_reversed;
}

uint8_t NVMWrapper::get_mod_pos(bool is_a) {
    if (!mod_data_stored) return 0;
    return (is_a)? mod_mem.a_idx : mod_mem.b_idx;
}

void NVMWrapper::set_mod_pos(bool is_a, uint8_t mod_pos) {
    if (is_a) mod_mem.a_idx = mod_pos;
    else mod_mem.b_idx = mod_pos;
}

void NVMWrapper::save_mod_pos() {
    mod_mem.data_exists = 'Y';
    if (saved_a == mod_mem.a_idx && saved_b == mod_mem.b_idx && saved_follow == mod_mem.follow_on) return;
    EEPROM.put(sizeof(ConfigMemoryLayout) + 1, mod_mem);
    EEPROM.commit();
    saved_a = mod_mem.a_idx;
    saved_b = mod_mem.b_idx;
    saved_follow = mod_mem.follow_on;
}

void NVMWrapper::set_follow(bool follow) {
    mod_mem.follow_on = follow;
}

bool NVMWrapper::get_follow() {
    if (!mod_data_stored) return false;
    return mod_mem.follow_on;
}
