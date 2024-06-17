"""
Frequency response for input RC filter

cutoff: 50Hz
fs:     1kHz 
"""

from scipy import signal
from scipy import pi
from matplotlib.ticker import ScalarFormatter
import matplotlib.pyplot as plt
import numpy as np

# 𝑓𝑐=1/2𝜋𝑅𝐶
# 2𝜋𝑅𝐶 = 1/𝑓𝑐
# 𝑅 = 1/𝐶.2𝜋𝑓𝑐

TAU = 2*pi
N = 1       # order of filter
f0 = 50     # cutoff frequency (Hz)
fs = 1000   # sampling frequency (Hz)
aliasing = 3/5 # max. allowable aliasing noise with Schmitt trigger with Vʟᴏᴡ=1.0V Vʜɪɢʜ=4.0V

C = 3.3/10000000 # 0.33uF
R = 1/(C*TAU*f0)

print('R:',R)
print('C:',C)

# Butterworth filter (analog)
b, a = signal.butter(N, TAU*f0, 'lowpass', analog=True)
w, h = signal.freqs(b, a)
f = w/TAU

# Plot frequency response (dB)
plt.semilogx(f, 20 * np.log10(abs(h)))
plt.title('Butterworth analog filter frequency response')
plt.xlabel('Frequency [Hz]')
plt.ylabel('Amplitude [dB]')
plt.margins(0, 0.1)
plt.grid(which='both', axis='both')
plt.axvline(f0, color='green')  # cutoff frequency
plt.gca().xaxis.set_major_formatter(ScalarFormatter())
plt.show()

# Plot extended frequency response (voltage)
w, h = signal.freqs(b, a, worN=TAU*np.logspace(0, 3, num=500, base=10))
f = w/TAU

plt.semilogx(f, abs(h))
plt.title('Butterworth analog filter frequency response')
plt.xlabel('Frequency [Hz]')
plt.ylabel('Amplitude [V]')
plt.margins(0, 0.1)
plt.grid(which='both', axis='both')
plt.axvline(f0, color='green')      # cutoff frequency
plt.axvline(fs/2, color='green')    # Nyquist freqency (internal LPF)
plt.axhline(aliasing, color='red')  # max. allowable aliasing amplitude
plt.gca().xaxis.set_major_formatter(ScalarFormatter())
plt.show()


