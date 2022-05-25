# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk

Plots the heartrate in realtime. Start from
within atty-scope.
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading
import ecg_analysis

# Reads from 1st filtered selected channel (comes after all the
# unfiltered packets).
# Filter settings:
# For ECG set the gain to approx 1000, Highpass at 1Hz and mains notch.
channel = 11

# sampling rate
fs = 250

heartrate_detector = ecg_analysis.heartrate_detector(fs)

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
    global channel
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
        ringbuffer.append(values[channel])
        

# start reading data from socket
t = threading.Thread(target=readStdin)
t.start()

# now let's plot the data
fig, ax = plt.subplots()
# that's our plotbuffer
plotbuffer = np.zeros(60)
# plots an empty line
line, = ax.plot(plotbuffer)
# axis
ax.set_ylim(0, 200)
ax.set_xlabel('Heartbeat #')
ax.set_ylabel('Beats per minute')
ax.set_title('Heartrate')

# receives the data from the generator below
def update(data):
    global heartrate_detector
    global plotbuffer
    
    for d in data:
        heartrate_detector.detect(d)
        rate = heartrate_detector.bpm
        heartrate_detector.bpm = 0
        
        # add new data to the buffer
        if rate>0:
            plotbuffer=np.append(plotbuffer,rate)
        
    # only keep the 60 newest ones and discard the old ones
    plotbuffer=plotbuffer[-60:]
    # set the new 60 points
    line.set_ydata(plotbuffer)
    return line,

# this checks in an endless loop if there is data in the ringbuffer
# of there is data then emit it to the update funnction above
def data_gen():
    global ringbuffer
    #endless loop which gets data
    while True:
        # check if data is available
        if not ringbuffer == []:
            result = ringbuffer
            ringbuffer = []
            yield result

# start the animation
ani = animation.FuncAnimation(fig, update, data_gen, interval=100)

# show it
plt.show()

# stop the thread which reads the data
doRun = False
# wait for it to finish
t.join()

print("finished")
