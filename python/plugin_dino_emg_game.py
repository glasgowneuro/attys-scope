#!/usr/bin/python3
"""
Requires pyqtgraph.

Copyright (c) 2018-2022, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

Plots the EMG Amplitude from a signal above 5Hz.
"""

channel = 7 # 1st ADC unfiltered

gain = 50000

from dinogame import DinoGame

import sys
import numpy as np
from scipy import signal
import iir_filter
from attys_scope_plugin_comm import AttysScopeReader

# The high- and lowpass filters can only be set after
# the sampling rate is known
hpiir = False
lpiir = False
game = False
w = 2000

# init the filters once we know the sampling rate
def callbackFs(fs):
    global hpiir,lpiir
    hpfc = 5 # highpass freq
    hpsos = signal.butter(2, hpfc/fs*2, output='sos', btype='highpass')
    lpfc = 1 # lowpass freq
    lpsos = signal.butter(2, lpfc/fs*2, output='sos')
    hpiir = iir_filter.IIR_filter(hpsos)
    lpiir = iir_filter.IIR_filter(lpsos)
    w = fs * 5

# process data with the filters set up
def callbackData(data):
    global game,w
    v = data[channel]
    v = hpiir.filter(v)
    v = np.abs(v)
    v = lpiir.filter(v)
    v = v * gain
    l = " " * 40
    l = l[:20]+"|"+l[20:]
    x = v * 20
    if x>40:
        x = 40
    if v < 1:
        c = '>'
    if v == 1:
        c = '!'
    if v > 1:
        c = '<'
    l = l[:int(x)]+c+l[int(x):]
    print(l)
    if w > 0:
        w = w - 1
        if w < 1:
            game.start_running()
        return
    if v > 1:
        game.jump()

game = DinoGame()
    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

game.start()

attysScopeReader.stop()

print("Finished")
