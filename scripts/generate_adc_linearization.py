import os

path_to_tables = "/src/tables/"
file_name = "adc_linearization_table.h"

file_header = """// autogenerated by scripts/generate_adc_linearization.py
// do not modify!!
#include<stdint.h>
#ifndef ADC_LINEARIZATION_TABLE_H
#define ADC_LINEARIZATION_TABLE_H
"""
array_footer = "};\n"
file_footer = "#endif"


def linearize_adc(val: int):
    points = [
        (0, 0.75),
        (511, -2.75),
        (512, 6.75),
        (1023, 2.8),
        (1024, 2.1),
        (1535, -2.2),
        (1536, 5.6),
        (2047, 1.5),
        (2048, -1.5),
        (2559, -5.5),
        (2560, 3),
        (3071, -0.9),
        (3072, -2),
        (3583, -6),
        (3584, 3),
        (4096, -1.2),
    ]
    x_pts = [pt[0] for pt in points]
    y_pts = [pt[1] for pt in points]
    
    idx = x_pts.index(max([x_val for x_val in x_pts if x_val <= val]))
    slope = (y_pts[idx+1] - y_pts[idx]) / (x_pts[idx+1] - x_pts[idx])
    offset = y_pts[idx]
    error = slope*(val - x_pts[idx]) + offset
    return val + error

def main():
    file_path = f"{os.getcwd()}{path_to_tables}{file_name}"
    with open(file_path, 'w') as fp:
        fp.write(file_header)
        fp.write("const uint16_t adc_tf_lut[4096] = {")
        for i in range(4096):
            corrected_i = round(linearize_adc(i))
            arr_write_item(fp, i, corrected_i, 32, 4096)
        fp.write(array_footer)
        fp.write(file_footer)
    
    
def arr_write_item(f, idx, val_to_write, vals_per_line, arr_len):
    if idx != arr_len-1:
        f.write(f"{val_to_write}, ")
    else:
        f.write(f"{val_to_write}")
    
    if (idx + 1) % vals_per_line == 0:
        f.write("\n")
        
        
if __name__ == "__main__":
    main()