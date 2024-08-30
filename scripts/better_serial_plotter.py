import serial as s
import matplotlib.pylab as plt
import numpy as np
import math as m

port_str = "COM10"
display_len_s = 20
print_freq_hz = int(1000 / 50)

def main():
    ser = s.Serial(port=port_str, baudrate=9600)
    
    arr_len = display_len_s * print_freq_hz
    x_vals = np.array(range(arr_len))
    y_vals = np.zeros(x_vals.shape)
    
    plt.ion()
    graph = plt.plot(x_vals, y_vals)[0]
    plt.ylim([0, 2**12])
    avg_arr, std_arr, rng_arr = [], [], []
    
    loop_count, ilc = 0, 0
    while True:
        new_val = int(ser.readline().strip())
        y_vals = np.insert(y_vals[1:], y_vals.size-1, new_val)
        graph.set_ydata(y_vals)
        plt.draw()
        plt.pause(0.0001)
        
        loop_count += 1
        if loop_count % print_freq_hz == 0:
            stdev = np.std(y_vals)
            center = np.average(y_vals)
            rng = max(y_vals)- min(y_vals)
            if loop_count < print_freq_hz * 30: continue
            std_arr.append(stdev)
            avg_arr.append(center)
            rng_arr.append(rng)
            print(f"avg: {center}, std: {stdev}, range: {rng}")
            ilc += 1
            if ilc % 10 == 0:
                print(f"avg avg: {np.average(avg_arr)}, std: {np.average(std_arr)}, range: {np.average(rng_arr)}")
            

if __name__ == "__main__":
    main()