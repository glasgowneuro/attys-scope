# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk

Plots the frequency spectrum of the 1st selected channel in
Attys scope

"""

import socket
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading

# in ms the update
updateRate = 1000

# read from channels:

# Title for Channel1:
titleChannel1 = "Kirsty's EEG"
# From data Column:
channel1 = 7

# Title for Channel2:
titleChannel2 = "Katie's EEG"
# From data Column:
channel2 = 28

# minimal frequency detectable in Hz
minF = 1000.0 / updateRate

# socket connection to attys_scope
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
listen_addr = ("",65000)
s.bind(listen_addr)
f = s.makefile()

#That's our ringbuffer which accumluates the samples
#It's emptied every time when the plot window below
#does a repaint
ringbuffer = []

# for the thread below
doRun = True

# This reads the data from the socket in an endless loop
# and stores the data in a buffer
def readSocket():
    global ringbuffer
    while doRun:
        # check if data is available
        data = f.readline()
        values = np.array(data.split(','),dtype=np.float32)
        ringbuffer.append(values)
        

# start reading data from socket
t = threading.Thread(target=readSocket)
t.start()

# now let's plot the data
fig, (ax1,ax2) = plt.subplots(2,1)
fig.subplots_adjust(hspace=0.5)
ax1.set_xlim(minF,125)
ax2.set_xlim(minF,125)
ax1.set_xlabel('')
ax1.set_ylabel('Amplitude/V')
ax1.set_xlabel('Frequency/Hz')
ax2.set_xlabel('Frequency/Hz')
ax2.set_ylabel('Amplitude/V')
ax1.set_title(titleChannel1)
ax2.set_title(titleChannel2)

# empty axes
e = np.array([1,2])
line1, = ax1.semilogx(e,e)
line2, = ax2.semilogx(e,e)

# receives the data from the generator below
def update(data):
    global ringbuffer
    global channel1
    global channel2
    global line1
    global line2
    global ax1,ax2, fig
    global titlePlot1
    global titlePlot2
    # axis
    spectrum1 = np.fft.rfft(np.array(ringbuffer)[:,channel1])
    spectrum2 = np.fft.rfft(np.array(ringbuffer)[:,channel2])
    # absolute value
    spectrum1 = np.abs(spectrum1)/len(spectrum1)
    spectrum2 = np.abs(spectrum2)/len(spectrum2)
    spectrum1[0] = 0
    spectrum2[0] = 0
    line1.set_data(np.linspace(0,125,len(spectrum1)), spectrum1)
    line2.set_data(np.linspace(0,125,len(spectrum2)), spectrum2)
    # set new max
    ax1.set_ylim(0,spectrum1.max()*1.2)
    ax2.set_ylim(0,spectrum2.max()*1.2)
    ringbuffer = []
    # return the line
    return [line1,line2]

# start the animation
ani = animation.FuncAnimation(fig, update, interval=1000)

# show it
plt.show()

# stop the thread which reads the data
doRun = False
# wait for it to finish
t.join()

# close the file and socket
f.close()
s.close()

print("finished")
