# -*- coding: utf-8 -*-
"""
@author: Bernd Porr, mail@berndporr.me.uk

Plots a realtime bargraph.

Start from within attys-scope.

"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from attys_scope_plugin_comm import AttysScopeReader

# channels
ch1 = 11
ch2 = 12

# init the amplitudes
amplitudes = np.zeros(2)

# get a pointer to a plotwindow
fig = plt.figure()

objects = ('Ch1', 'Ch2')
xpos = np.arange(len(objects))
rects = plt.bar(xpos,amplitudes)
plt.xticks(xpos, objects)
plt.ylabel('Amplitudes/V')
plt.ylim((0,2))

# receives the data from the generator below
def update(data):
    global rects
    for h,r in zip(amplitudes,rects):
        r.set_height(h)
    return rects

def callBack(data):
    global amplitudes
    amplitudes[0] = np.abs(data[ch1])
    amplitudes[1] = np.abs(data[ch2])

# start the animation
ani = animation.FuncAnimation(fig, update, interval=100)

attysScopeReader = AttysScopeReader(callBack)
attysScopeReader.start()

# show it
plt.show()

attysScopeReader.stop()

print("finished")
