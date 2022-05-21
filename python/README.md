# Python code examples for attys-scope
Python scripts to demonstrate how to process data with
attys-scope.

## plot_demo_data.py
Plots data from a file recorded by attys_scope.

## Python realtime plugins

Start these plugins from Attys-scope by selecting `Python->Run`.

### plugin_plot.py
Realtime plot of data: Plots the data from the 1st selected
channel.

### plugin_bargraph.py
Shows in realtime the amplitude of the 1st two selected
signals in attys_scope

### plugin_spectrum.py
Shows in realtime the Fourier Spectrum of the 1st channel selected
in a semilog scale.

### plugin_heartrate.py
Realtime heartrate detection. The actual detector is in the
module ecg_analysis.py


Alpha waves in an EEG while closing the eyes:

![alt tag](realtime_spectrum_eeg_alpha_waves.png)
