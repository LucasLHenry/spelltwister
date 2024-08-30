import serial as s
import matplotlib.pylab as plt
import numpy as np

port_str = "COM10"
display_len_s = 2
print_freq_hz = 1000

def main():
    ser = s.Serial(port=port_str, baudrate=9600)
    
    arr_len = display_len_s * print_freq_hz
    x_vals = np.array(range(arr_len))
    y_vals = np.zeros(x_vals.shape)
    
    plt.ion()
    graph = plt.plot(x_vals, y_vals)[0]
    
    while True:
        new_val = int(ser.readline().strip())
        print(new_val)
        np.roll(y_vals, new_val, -1)
        graph.set_ydata(y_vals)
        plt.draw()
        plt.pause(1/print_freq_hz)

if __name__ == "__main__":
    main()