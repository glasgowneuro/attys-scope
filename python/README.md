# Python code examples for attys-scope
Python scripts to demonstrate how to process data with
attys-scope.

All realtime demos read the data via the UDP port 65000.
Make sure attys_scope's broadcast is on.

## plot_demo_data.py
Plots data from a file recorded by attys_scope.

![alt tag](plot_demo_data_screenshot.png)

## Python realtime plugins

Start these plugins from Attys-scope by selecting `Python->Run`.

### realtime_plot.py
Realtime plot of data: Plots the data from the 1st selected
channel.

### realtime_bargraph.py
Shows in realtime the amplitude of the 1st two selected
signals in attys_scope

### realtime_spectrum.py
Shows in realtime the Fourier Spectrum of the 1st channel selected
in a semilog scale.

![alt tag](realtime_spectrum_eeg_alpha_waves.png)

### realtime_heartrate.py
Realtime heartrate detection. The actual detector is in the
module ecg_analysis.py
