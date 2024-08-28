import ctypes as ct
import os
import matplotlib.pyplot as plt
from math import cos, pi

def main():
    bits = 16
    sample_rate_kHz = 44.1
    
    test_freq_hz = 30
    num_cycles = 4
    in_arr = generate_sample_wave(sample_rate_kHz, bits, test_freq_hz, num_cycles, "SINE")
    
    out_file_name = "algos"
    paths_to_files = ["../src/wave_algos/simple_mod_algos.cpp", 
                      "../src/constants.hpp", 
                      "../src/tables/transfer_functions/sine.h",
                      "../src/tables/transfer_functions/analog_pulse.h"]
    
    # generate dll file
    for i, path in enumerate(paths_to_files):
        os.system(f"gcc -c {path} -o c{i}.o")
        os.system(f"gcc -shared -o {out_file_name}.dll c{i}.o")
        # os.system(f"rm c{i}.o")
    
    # os.system(f"gcc -fPIC -shared -o {out_file_name}.dll {path_to_cpp}")
    
    algos_c = ct.CDLL(f"{out_file_name}.dll")
    pass

def generate_sample_wave(sr_kHz, bits, freq_hz, num_cycles, type) -> list[int]:
    max_val = (2 ** bits) - 1
    samples_per_cycle = sr_kHz * 1000 / freq_hz
    arr_len = int(samples_per_cycle * num_cycles)
    arr = []
    if type == "TRI":
        going_up = True
        increment = max_val / (samples_per_cycle / 2)  # rise over run
        val = 0
        for _ in range(arr_len):
            arr.append(int(val))
            if going_up: val += increment
            else: val -= increment
            if (val >= max_val) or (val <= 0):
                going_up = not going_up
    elif type == "SQR":
        for i in range(arr_len):
            if i % samples_per_cycle < (samples_per_cycle / 2):
                arr.append(0)
            else: arr.append(max_val)
    elif type == "SINE":
        angular_freq = 2*pi / samples_per_cycle
        for i in range(arr_len):
            val = ((cos(angular_freq*i) / -2) + 1) * max_val
            arr.append(int(val))
    elif type == "SAW":
        increment = max_val / samples_per_cycle  # rise over run
        val = 0
        for _ in range(arr_len):
            arr.append(int(val))
            val += increment
            val %= max_val
    else:  # silence
        for _ in range(arr_len):
            arr.append(int(max_val / 2))
    
    return arr

if __name__ == "__main__":
    main()