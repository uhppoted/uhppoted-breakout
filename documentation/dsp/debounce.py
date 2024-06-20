#!python3

import matplotlib.pyplot as plt
import numpy as np

def main():
    print("debounce")
    t = np.linspace(0, 20, num=20*10)
    v = list(map(V,t))

    plt.plot(t, v)
    plt.title('DEBOUNCE')
    plt.xlabel('t [ms]')
    plt.ylabel('in [V]')
    plt.margins(0, 0.1)
    plt.grid(which='both', axis='both')
    plt.show()

def V(t):
    if t < 5:
        return 1.0
    else:
        return 0.0

if __name__ == '__main__':
    main()
