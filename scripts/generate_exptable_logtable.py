import os
import math as m

# this file creates the phasor tables for the waveformers, as well as some
# constants used by them (max and min freq for slow and fast phasors)

# set these!
table_bits = 12
item_bits = 16
amplitude = 0.8

path_to_tables = "/src/tables/"
file_name = "curve_tables.h"

file_header = """// autogenerated by scripts/generate_exptable_logtable.py
// do not modify!!
#include<stdint.h>
#ifndef CURVE_TABLES_H
#define CURVE_TABLES_H
"""
array_footer = "};\n"
file_footer = "#endif  // CURVE_TABLES_H"

def main():
    file_path = f"{os.getcwd()}{path_to_tables}{file_name}"
    max_val = (2 ** item_bits) - 1
    table_len = 2 ** table_bits
    scale_val = m.log(table_len) / (table_len - 1)
    
    with open(file_path, 'w') as f:
        f.write(file_header)
        
        f.write(f"const uint32_t curve_table_bits = {table_bits};\n")
        
        f.write(f"const uint16_t exptable[{table_len}] = {{")
        for i in range(table_len):
            val =  max_val * (m.exp(scale_val * i) - 1) / (table_len - 1)
            val = ((val - max_val/2) * amplitude) + max_val/2
            arr_write_item(f, i, int(val), 32, table_len)
        f.write(array_footer)
        
        f.write(f"const uint16_t logtable[{table_len}] = {{")
        for i in range(table_len):
            val =  max_val - max_val * (m.exp(scale_val * (table_len-1 - i)) - 1) / (table_len - 1)
            val = ((val - max_val/2) * amplitude) + max_val/2
            arr_write_item(f, i, int(val), 32, table_len)
        f.write(array_footer)
        
        f.write(f"const uint16_t lintable[{table_len}] = {{")
        for i in range(table_len):
            val =  max_val*i / (table_len - 1)
            val = ((val - max_val/2) * amplitude) + max_val/2
            arr_write_item(f, i, int(val), 32, table_len)
        f.write(array_footer)
        
        f.write(file_footer)

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