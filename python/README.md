# Python code examples for attys-scope
Python scripts to demonstrate how to process data with
attys-scope.

## plot_demo_data.py
Plots data from a file recorded by attys_scope.

## Python realtime plugins

Start these plugins from Attys-scope by selecting `Python->Run`.

### plugin_realtime_plot.py
Realtime plot of data: Plots the data from the 1st selected
channel.

### plugin_power.py
Squares the 1st analogue channel and then lowpass filters it.
This is a good demo how to do realtime filtering on the data and displaying it.

### plugin_bargraph.py
Shows in realtime the amplitude of the 1st two selected
signals in attys_scope

### plugin_spectrum.py
Shows in realtime the Fourier Spectrum of the 1st channel selected
in a semilog scale.

### plugin_heartrate.py
Realtime heartrate detection. The actual detector is in the
module ecg_analysis.py

### plugin_emg.py
Performs highpass filtering of the signal, takes the power and then does
a lowpass filtering. It also shows how to use buttons in the PyPlot environment.

### Writing your own python plugin
`attys_scope_plugin_comm.py` is the API to communicate with attys-scope. It has
two callbacks: one for the data and one for the sampling rate detected.
The best approach is to hack one of the plugins above and customise it to your needs.
To see errors produced by your script you can open the python console at `Python->Console`.
Under Linux the python output is also printed in Linux console where attys-scope is running.

Alpha waves in an EEG while closing the eyes:

![alt tag](realtime_spectrum_eeg_alpha_waves.png)
