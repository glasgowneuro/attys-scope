# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk

Plots the frequency spectrum of the 1st selected channel in
attys-scope (and its filtering).
Start from within attys-scope.
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import scipy.signal as signal
from attys_scope_plugin_comm import AttysScopeReader


# sampling rate, updated in callback
fs = False

# update rate in ms
updateRate = 1000

# Title for the Channel:
titleChannel = "Frequency spectrum"
# From data Column:
channel = 11

# minimal frequency detectable in Hz
minF = 1000.0 / updateRate

#That's our ringbuffer which accumluates the samples
#It's emptied every time when the plot window below
#does a repaint
ringbuffer = []

# now let's plot the data
fig, ax = plt.subplots(1,1)
ax.set_xlim(minF,125)
ax.set_xlabel('')
ax.set_ylabel('Amplitude/V^2')
ax.set_xlabel('Frequency/Hz')
ax.set_title(titleChannel)

# empty axes
e = np.array([1,2])
line, = ax.semilogx(e,e)

# receives the data from the generator below
def update(data):
    global ringbuffer
    global channel
    global line
    global ax, fig
    global titlePlot
    # axis
    f, spectrum = signal.periodogram(np.array(ringbuffer)[:,channel], fs, scaling='spectrum', nfft=fs)
    line.set_data(f, spectrum)
    # set new max
    ax.set_ylim(0,spectrum.max()*1.2)
    ringbuffer = []
    # return the line
    return line

def callbackFs(_fs):
    global fs
    fs = _fs

def callbackData(data):
    global ringbuffer
    ringbuffer.append(data)
    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

# start the animation
ani = animation.FuncAnimation(fig, update, interval=2000)

# show it
plt.show()

attysScopeReader.stop()

print("finished")
