# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk

Plots data in realtime with matplotlib.
Start from attys-scope.
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading
from scipy import signal
import iir_filter

fs = 250
hpfc = 5
hpsos = signal.butter(2, hpfc/fs*2, output='sos')
lpfc = 1
lpsos = signal.butter(2, lpfc/fs*2, output='sos')

hpiir = iir_filter.IIR_filter(hpsos)
lpiir = iir_filter.IIR_filter(lpsos)

# attys channel to be plotted from 
channel = 7

#That's our ringbuffer which accumluates the samples
#It's emptied every time when the plot window below
#does a repaint
ringbuffer = []

# for the thread below
doRun = True

# This reads the data from the socket in an endless loop
# and stores the data in a buffer
def readstdin():
    global ringbuffer
    global channel
    global doRun
    global ani
    while doRun:
        data = sys.stdin.readline()
        if not data:
            print("Stopping")
            doRun = False
            ani.event_source.stop()
            break
        values = np.array(data.split(','),dtype=np.float32)
        values = hpiir.filter(values)
        values = values*values
        values = lpiir.filter(values)
        ringbuffer.append(values[channel])

# receives the data from the generator below
def update(data):
    global plotbuffer
    # add new data to the buffer
    plotbuffer=np.append(plotbuffer,data)
    # only keep the 500 newest ones and discard the old ones
    plotbuffer=plotbuffer[-500:]
    # set the new 500 points of channel 9
    line.set_ydata(plotbuffer)
    return line,

# this checks in an endless loop if there is data in the ringbuffer
# of there is data then emit it to the update funnction above
def data_gen():
    global ringbuffer
    global doRun
    #endless loop which gets data
    while True:
        # check if data is available
        if not ringbuffer == []:
            result = ringbuffer
            ringbuffer = []
            yield result


# now let's plot the data
fig, ax = plt.subplots()
# that's our plotbuffer
plotbuffer = np.zeros(500)
# plots an empty line
line, = ax.plot(plotbuffer)
# axis
ax.set_ylim(0, 3)

# start the animation
ani = animation.FuncAnimation(fig, update, data_gen, interval=100)

# start reading data from socket
t = threading.Thread(target=readstdin)
t.start()

# show it
plt.show()

# stop the thread which reads the data
doRun = False
# wait for it to finish
t.join()

print("finished")
