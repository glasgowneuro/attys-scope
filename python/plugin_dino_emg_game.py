#!/usr/bin/python3
"""
Requires pyqtgraph.

Copyright (c) 2018-2022, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

Plots the EMG Amplitude from a signal above 5Hz.
"""

channel = 7 # 1st ADC unfiltered

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
    if w > 0:
        w = w - 1
        if w < 1:
            game.start_running()
        print("Wait")
        return
    if v > 0.0001:
        print("jump")
        game.jump()
    print(v)

game = DinoGame()
    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

game.start()

attysScopeReader.stop()

print("Finished")
