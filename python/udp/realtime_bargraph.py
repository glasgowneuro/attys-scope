# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk


"""

import socket
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading

# socket connection to attys_scope
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
listen_addr = ("",65000)
s.bind(listen_addr)
f = s.makefile()

# init the amplitudes
amplitudes = np.zeros(2)

# for the thread below
doRun = True

# read data from the udp socket into two variables
# note you need to have at least two analoge channels on
# this just overwrites the amplitudes as fast as possible
# for an oscilloscope plot we need a proper ringbuffer here!
def readSocket():
    global amplitudes 
    while doRun:
        # check if data is available
        data = f.readline()
        values = np.array(data.split(','),dtype=np.float32)
        values = np.fabs(values)
        amplitudes = values[9:11]
        

# start reading it in another thread
t = threading.Thread(target=readSocket)
t.start()

# get a pointer to a plotwindow
fig = plt.figure()

objects = ('Ch1', 'Ch2')
xpos = np.arange(len(objects))
rects = plt.bar(xpos,amplitudes)
plt.xticks(xpos, objects)
plt.ylabel('Amplitudes/V')
plt.ylim((0,0.0001))

# receives the data from the generator below
def update(data):
    global rects
    for h,r in zip(data,rects):
        r.set_height(h)
    return rects

# our generator doesnt do much except than getting the
# amplitudes. Again, if we wanted to do a lineplot then
# we needed to read it from a ringbuffer instead
def data_gen():
    global amplitudes
    #endless loop which gets data
    while True:
        yield amplitudes

# start the animation
ani = animation.FuncAnimation(fig, update, data_gen, interval=100)

# show it
plt.show()
doRun = False
t.join()

f.close()
s.close()

print("finished")
