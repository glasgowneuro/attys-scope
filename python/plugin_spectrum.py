# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk

Plots the frequency spectrum of the 1st selected channel in
attys-scope.
Start from attys-scope.
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading
import scipy.signal as signal

# sampling rate
fs = 250

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

# for the thread below
doRun = True

# This reads the data from the socket in an endless loop
# and stores the data in a buffer
def readStdin():
    global ringbuffer
    global doRun
    global ani
    while doRun:
        # check if data is available
        data = sys.stdin.readline()
        if not data:
            print("Stopping")
            doRun = False
            ani.event_source.stop()
            break
        values = np.array(data.split(','),dtype=np.float32)
        ringbuffer.append(values)

# start reading data from socket
t = threading.Thread(target=readStdin)
t.start()

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

# start the animation
ani = animation.FuncAnimation(fig, update, interval=2000)

# show it
plt.show()

# stop the thread which reads the data
doRun = False
# wait for it to finish
t.join()

print("finished")
