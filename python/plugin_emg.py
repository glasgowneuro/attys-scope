#!/usr/bin/python3
"""
Requires pyqtgraph.

Copyright (c) 2018-2022, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

Plots the EMG Amplitude from a signal above 5Hz.
"""

channel = 7 # 1st ADC unfiltered

import sys
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtWidgets
import numpy as np
from scipy import signal
import iir_filter
from attys_scope_plugin_comm import AttysScopeReader

app = pg.mkQApp()

class QtPanningPlot:

    n = 500
    CH1 = 7
    CH2 = 8

    def __init__(self,title):
        self.title = title
        self.channelCallback = False
        self.mw = QtWidgets.QMainWindow()
        self.mw.setWindowTitle('EMG')
        self.mw.resize(800,600)
        self.cw = QtWidgets.QWidget()
        self.mw.setCentralWidget(self.cw)
        self.l = QtWidgets.QVBoxLayout()
        self.cw.setLayout(self.l)
        self.pw = pg.PlotWidget()
        self.l.addWidget(self.pw)
        self.pw.setYRange(0,1.5)
        self.pw.setLabel('bottom', 't/sec')
        self.plt = self.pw.plot()

        self.data = []
        # any additional initalisation code goes here (filters etc)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)
        l2 = QtWidgets.QHBoxLayout()
        ccb1 = QtWidgets.QPushButton("Ch1")
        ccb1.clicked.connect(self.ch1Callback)
        ccb2 = QtWidgets.QPushButton("Ch2")
        ccb2.clicked.connect(self.ch2Callback)
        l2.addWidget(ccb1)
        l2.addWidget(ccb2)
        self.l.addLayout(l2)
        self.mw.show()

    def ch1Callback(self):
        if self.channelCallback:
            self.channelCallback(self.CH1)
            self.pw.setLabel('left', 'Ch1/V')
        
    def ch2Callback(self):
        if self.channelCallback:
            self.channelCallback(self.CH2)
            self.pw.setLabel('left', 'Ch2/V')
        
    def update(self):
        self.data=self.data[-self.n:]
        if self.data:
            d = np.hstack(self.data)
            t = np.linspace(0,len(d)/self.fs,len(d))
            self.pw.setXRange(0,len(d)/self.fs)
            self.plt.setData(x=t,y=d)

    def addData(self,d):
        self.data.append(d)

    def setFs(self,fs):
        self.fs = fs

    def setChannelCallback(self,channelCallback):
        self.channelCallback = channelCallback
        self.ch1Callback()


##############################################################################
## main

        
qtPanningPlot = QtPanningPlot("EMG amplitude")

# The high- and lowpass filters can only be set after
# the sampling rate is known
hpiir = False
lpiir = False

# init the filters once we know the sampling rate
def callbackFs(fs):
    global hpiir,lpiir
    hpfc = 5 # highpass freq
    hpsos = signal.butter(2, hpfc/fs*2, output='sos', btype='highpass')
    lpfc = 1 # lowpass freq
    lpsos = signal.butter(2, lpfc/fs*2, output='sos')
    hpiir = iir_filter.IIR_filter(hpsos)
    lpiir = iir_filter.IIR_filter(lpsos)
    qtPanningPlot.setFs(fs)

def callbackSetChannel(c):
    global channel
    channel = c

qtPanningPlot.setChannelCallback(callbackSetChannel)

# process data with the filters set up
def callbackData(data):
    v = data[channel]
    v = hpiir.filter(v)
    v = np.abs(v)
    v = lpiir.filter(v)
    qtPanningPlot.addData(v)

    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

# showing all the windows
pg.exec()

attysScopeReader.stop()

print("Finished")
