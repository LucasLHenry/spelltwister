import ctypes
import os
import matplotlib.pyplot as plt
from math import cos, pi

def main():
    bits = 16
    sample_rate_kHz = 44.1
    
    test_freq_hz = 30
    num_cycles = 4
    in_arr = generate_sample_wave(sample_rate_kHz, bits, test_freq_hz, num_cycles, "SINE")
    plt.plot(in_arr)
    plt.show()

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