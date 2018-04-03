# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk

Reads from a thermocouple on channel 2 of the
Attys. Uses channel one's internal temperature
sensor for the cold junction temperature.

"""

import socket
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading

# set this to the zero value reading of the attys from
# channel 2 by short circuting the ch2 and then noting
# the voltage
offset = 0.000827


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
    global channel
    while doRun:
        # check if data is available
        data = f.readline()
        allch = np.array(data.split(','),dtype=np.float32)
        #uncomment to see the offset voltage to set the offset
        #print(allch[8])
        thermocouple = (allch[8]-offset) / 39E-6
        # get the temperature from the internal sensor
        temperature = allch[7]
        # add it all together to get the absolute temperature
        abstemp = thermocouple + temperature
        ringbuffer.append(abstemp)
        

# start reading data from socket
t = threading.Thread(target=readSocket)
t.start()

# now let's plot the data
fig, ax = plt.subplots()
# that's our plotbuffer
plotbuffer = np.zeros(500)
# plots an empty line
line, = ax.plot(plotbuffer)
# axis
ax.set_ylim(0, 100)


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

# close the file and socket
f.close()
s.close()

print("finished")
