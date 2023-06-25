# Python code examples for attys-scope
Python scripts to demonstrate how to process data with
attys-scope.

## Prerequisites

```
pip install pyqtgraph
pip install PyQt5
pip install scipy
pip install numpy
pip install matplotlib
```

## plot_demo_data.py
Plots data from a file recorded by attys_scope.

## Python realtime plugins

Start these plugins from Attys-scope by selecting `Python->Run`.

### plugin_realtime_plot_matplotlib.py
Realtime plot of data using matplotlib.

### plugin_realtime_plot_pyqtgraph.py
Realtime plot of data using pyqtgraph which is much faster than matplotlib and
fully integrates into QT.

### plugin_bargraph.py
Shows in realtime the amplitude of the 1st two selected
signals in attys_scope

### plugin_spectrum.py
Shows in realtime the Fourier Spectrum of the 1st channel selected
in a semilog scale.

### plugin_ecg.py
Shows Einthoven I,II,III

Switch the attys-scope to *ECG mode* for both analogue channels.
Wire up the subject:
 - Neg: right shoulder
 - Pos: left hip
 - GND: left shoulder

### plugin_heartrate.py
Realtime heartrate detection. The actual detector is in the
module ecg_analysis.py

### plugin_emg.py
Performs highpass filtering of the signal, takes the power and then does
a lowpass filtering. It also shows how to use buttons in the PyPlot environment.

### plugin_dino_emg_game.py
The famous Dino Game controlled with EMG. Flex your muscle and make
the dino jump.
Requires: https://pypi.org/project/dino-game/

These are the processing stages of the EMG signal:

 1. Highpass at 1Hz: v=hp(v)
 2. Absolute value: v=abs(v)
 3. Lowpass at 5hz: v=lp(v)
 4. Amplified by factor `gain`: v=v*gain
 5. if v > 1 then jump!

Tweak the gain that the dino jumps when the muscle is flexed.

### plugin_eeg.py
Plots the EEG gamma, beta, alpha, theta and delta waves.

### Write your own python plugin
The best approach is to hack one of the plugins above and customise it to your needs.
`attys_scope_plugin_comm.py` is the API to communicate with attys-scope. It has
two callbacks: one for the data and one for the sampling rate detected.
To see errors produced by your script you can open the python console at `Python->Console`.
Under Linux the python output is also printed in Linux console where attys-scope is running.

Alpha waves in an EEG while closing the eyes:

![alt tag](realtime_spectrum_eeg_alpha_waves.png)
