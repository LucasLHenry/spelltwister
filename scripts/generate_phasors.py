import os

# this file creates the phasor tables for the waveformers, as well as some
# constants used by them (max and min freq for slow and fast phasors)

# set these!
min_vco_freq_hz = 30
min_lfo_env_period_s = 0.025
max_lfo_env_period_s = 10

# hardware determined
adc_bits = 12
arr_len = 2 ** adc_bits
voltage_max = 7
hz_phasor = 92291  # phasor that corresponds to 1Hz freq

path_to_tables = "/src/tables/"
file_name = "phasors.h"

file_header = """// autogenerated by scripts/generate_phasors.py
// do not modify!!
#include<stdint.h>
#ifndef PHASORS_TABLES_H
#define PHASORS_TABLES_H
"""
array_footer = "};\n"
file_footer = "#endif  // PHASORS_TABLES_H"

def main():
    file_path = f"{os.getcwd()}{path_to_tables}{file_name}"
    
    with open(file_path, 'w') as f:
        f.write(file_header)
        min_vco_phasor, max_vco_phasor = vco_phasor_writer(f)
        f.write(f"const uint32_t hz_phasor = {hz_phasor};\n")
        f.write(f"const uint32_t min_pha = {min_vco_phasor};\n")
        f.write(f"const uint32_t max_pha = {max_vco_phasor};\n")
        min_slow_phasor, max_slow_phasor = lfo_env_phasor_writer(f)
        f.write(f"const uint32_t min_slow_pha = {min_slow_phasor};\n")
        f.write(f"const uint32_t max_slow_pha = {max_slow_phasor};\n")
        scale_factor = int(2**(adc_bits + 9) / voltage_max)
        f.write(f"const uint32_t scale_factor = {scale_factor};\n")
        f.write(file_footer)
    
def vco_phasor_writer(f):
    min_pha, max_pha = 0, 0
    f.write(f"const uint32_t phasor_table[{arr_len}] = {{\n")
    for idx in range(arr_len):
        voltage = linear_map(idx, 0, arr_len-1, 0, voltage_max)
        freq = min_vco_freq_hz * (2 ** voltage)
        phasor = int(freq * hz_phasor)
        arr_write_item(f, idx, phasor, 16, arr_len)
        
        if idx == 0: min_pha = phasor
        if idx == arr_len - 1: max_pha = phasor
    f.write(array_footer)
    return min_pha, max_pha
    
def lfo_env_phasor_writer(f):
    min_pha, max_pha = 0, 0
    min_freq_hz, max_freq_hz = 1/max_lfo_env_period_s, 1/min_lfo_env_period_s
    
    f.write(f"const uint32_t slow_phasor_table[{arr_len}] = {{\n")
    for idx in range(arr_len):
        freq = linear_map(idx, 0, arr_len-1, min_freq_hz, max_freq_hz)
        phasor = int(freq * hz_phasor)
        arr_write_item(f, idx, phasor, 16, arr_len)
        
        if idx == 0: min_pha = phasor
        if idx == arr_len - 1: max_pha = phasor
    f.write(array_footer)
    return min_pha, max_pha

def linear_map(x, m1, M1, m2, M2):
    scale = (M2 - m2) / (M1 - m1)
    return (x - m1) * scale + m2

def arr_write_item(f, idx, val_to_write, vals_per_line, arr_len):
    if idx != arr_len-1:
        f.write(f"{val_to_write}, ")
    else:
        f.write(f"{val_to_write}")
    
    if (idx + 1) % vals_per_line == 0:
        f.write("\n")

if __name__ == "__main__":
    main()