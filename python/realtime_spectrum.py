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

# update rate in ms
updateRate = 1000

# Title for the Channel:
titleChannel = "Frequency spectrum"
# From data Column:
channel = 11

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
fig, ax = plt.subplots(1,1)
ax.set_xlim(minF,125)
ax.set_xlabel('')
ax.set_ylabel('Amplitude/V')
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
    spectrum = np.fft.rfft(np.array(ringbuffer)[:,channel])
    # absolute value
    spectrum = np.abs(spectrum)/len(spectrum)
    spectrum[0] = 0
    line.set_data(np.linspace(0,125,len(spectrum)), spectrum)
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

# close the file and socket
f.close()
s.close()

print("finished")
