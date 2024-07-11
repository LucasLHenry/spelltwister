#include "nvm_wrapper.h"

bool config_data_eq(ConfigData l, ConfigData r) {
    if (l.vo_offset != r.vo_offset)       return false;
    if (l.vo_scale != r.vo_scale)         return false;
    if (l.fm_offset != r.fm_offset)       return false;
    if (l.mod_offset != r.mod_offset)     return false;
    if (l.shape_offset != r.shape_offset) return false;
    if (l.ratio_offset != r.ratio_offset) return false;
    return true;
}

NVMWrapper::NVMWrapper() {
    EEPROM.begin(sizeof(MemoryLayout));
    mem = EEPROM.get(0, mem);
    data_in_eeprom = mem.data_exists == 'Y';
}

ConfigData NVMWrapper::get_config_data(bool is_a) {
    if (!data_in_eeprom) {
        return (is_a)? a_default_config_data : b_default_config_data;
    }

    return (is_a)? mem.a_data : mem.b_data;
}

ConfigData NVMWrapper::write_config_data(bool is_a, ConfigData& conf) {
    ConfigData* existing_data = (is_a)? &mem.a_data : &mem.b_data;
    if (!config_data_eq(conf, *existing_data)) {
        *existing_data = conf;
    }
}

void NVMWrapper::save_data() {
    EEPROM.commit();
}


// void update_values_from_config(LedRing& ring, Module& A, Module& B) {
//     char c_buf;
//     uint16_t idx_buf;
//     ConfigData conf_buf;

//     encoder_flag_storage.read(c_buf);
//     if (c_buf == config_exists_flag) {
//         a_idx_storage.read(idx_buf);
//         ring.a_idx = static_cast<uint16_t>(idx_buf);
//         ring.a_pos_raw = static_cast<uint16_t>(idx_buf) << ENC_DIV;

//         b_idx_storage.read(idx_buf);
//         ring.b_idx = static_cast<uint16_t>(idx_buf);
//         ring.b_pos_raw = static_cast<uint16_t>(idx_buf) << ENC_DIV;
//     } else {
//         // DEFAULT VALUES
//         ring.a_idx = 0;
//         ring.a_pos_raw = 0;

//         ring.b_idx = 0;
//         ring.b_pos_raw = 0;
//     }

//     calibration_flag_storage.read(c_buf);
//     if (c_buf == config_exists_flag) {
//         a_config_data_storage.read(conf_buf);
//         A.configs = conf_buf;

//         b_config_data_storage.read(conf_buf);
//         B.configs = conf_buf;
//     } else {
//         A.configs = a_default_config_data;
//         B.configs = b_default_config_data;
//     }
// }

// void write_encoder_to_config(LedRing& ring) {
//     uint16_t buf;
//     uint16_t out_buf;
//     char c_buf;

//     a_idx_storage.read(buf);
//     if (buf != ring.a_idx) {
//         out_buf = ring.a_idx;
//         a_idx_storage.write(out_buf);
//     }

//     b_idx_storage.read(buf);
//     if (buf != ring.b_idx) {
//         out_buf = ring.b_idx;
//         b_idx_storage.write(out_buf);
//     }

//     encoder_flag_storage.read(c_buf);
//     if (c_buf != config_exists_flag) encoder_flag_storage.write(config_exists_flag);
// }

// void write_calibration_to_config(Module& A, Module& B) {
//     ConfigData buf;
//     ConfigData out_buf;
//     char c_buf;

//     a_config_data_storage.read(buf);
//     if (!configData_eq(buf, A.configs)) {
//         out_buf = A.configs;
//         a_config_data_storage.write(out_buf);
//     }

//     b_config_data_storage.read(buf);
//     if (!configData_eq(buf, B.configs)) {
//         out_buf = B.configs;
//         b_config_data_storage.write(out_buf);
//     }

//     calibration_flag_storage.read(c_buf);
//     if (c_buf != config_exists_flag) calibration_flag_storage.write(config_exists_flag);
// }