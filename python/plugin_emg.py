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
from pyqtgraph.Qt import QtCore, QtGui
import numpy as np
from scipy import signal
import iir_filter
from attys_scope_plugin_comm import AttysScopeReader
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

# create a global QT application object
app = QtGui.QApplication(sys.argv)


class QtPanningPlot:

    n = 500
    CH1 = 7
    CH2 = 8

    def __init__(self,title):
        self.title = title
        self.channelCallback = False
        self.win = pg.GraphicsLayoutWidget()
        self.win.setWindowTitle(title)
        self.plt = self.win.addPlot()
        self.plt.setYRange(0,1.5)
        self.plt.setLabel('bottom', 't/sec')
        self.curve = self.plt.plot()
        self.data = []
        # any additional initalisation code goes here (filters etc)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)
        self.layout = QtGui.QGridLayout()
        self.win.setLayout(self.layout)
        proxy1 = QtGui.QGraphicsProxyWidget()
        ccb1 = QPushButton("Ch1")
        ccb1.clicked.connect(self.ch1Callback)
        proxy1.setWidget(ccb1)
        proxy2 = QtGui.QGraphicsProxyWidget()
        ccb2 = QPushButton("Ch2")
        ccb2.clicked.connect(self.ch2Callback)
        proxy2.setWidget(ccb2)
        p = self.win.addLayout(row=1, col=0)
        p.addItem(proxy1,row=1,col=1)
        p.addItem(proxy2,row=1,col=2)
        self.win.show()

    def ch1Callback(self):
        if self.channelCallback:
            self.channelCallback(self.CH1)
            self.plt.setLabel('left', 'Ch1/V')
        
    def ch2Callback(self):
        if self.channelCallback:
            self.channelCallback(self.CH2)
            self.plt.setLabel('left', 'Ch2/V')
        
    def update(self):
        self.data=self.data[-self.n:]
        if self.data:
            d = np.hstack(self.data)
            t = np.linspace(0,len(d)/self.fs,len(d))
            self.plt.setXRange(0,len(d)/self.fs)
            self.curve.setData(t,d)

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
app.exec_()

attysScopeReader.stop()

print("Finished")
