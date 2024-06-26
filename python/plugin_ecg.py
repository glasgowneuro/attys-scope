#!/usr/bin/python3
"""
Requires pyqtgraph.

Copyright (c) 2018-2023, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

Plots Einthoven I,II,III.
"""

CH1 = 7
CH2 = 8

# Bandstop frequency
fbs = 50 # Hz

import sys
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtWidgets
import numpy as np
from scipy import signal
import iir_filter
from attys_scope_plugin_comm import AttysScopeReader

app = pg.mkQApp()

class QtPanningPlot:

    # duration of the scrolling window
    twin = 2
    # scale in mV
    a = 2

    def __init__(self,title):
        self.title = title
        self.channelCallback = False
 
        self.pw = [False,False,False]
        self.plt = [False,False,False]
        self.data = [[],[],[]]
        ylabel = ["I","II","III"]

        self.mw = QtWidgets.QMainWindow()
        self.mw.setWindowTitle('ECG')
        self.mw.resize(800,800)
        self.cw = QtWidgets.QWidget()
        self.mw.setCentralWidget(self.cw)
        self.l = QtWidgets.QVBoxLayout()
        self.cw.setLayout(self.l)

        for i in range(3):
            self.pw[i] = pg.PlotWidget()
            self.l.addWidget(self.pw[i])
            self.pw[i].setYRange(-self.a,self.a)
            self.pw[i].setLabel('bottom', 't/sec')
            self.pw[i].setLabel('left',ylabel[i]+'/mV')
            self.l.addWidget(self.pw[i])
            self.plt[i] = self.pw[i].plot()

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)
        self.mw.show()

    def update(self):
        for i in range(3):
            self.data[i] = self.data[i][-int(self.twin*self.fs):]
            if self.data[i]:
                d = np.hstack(self.data[i])
                t = np.linspace(0,len(d)/self.fs,len(d))
                self.pw[i].setXRange(0,len(d)/self.fs)
                self.plt[i].setData(x=t,y=d)

    def addData(self,d1,d2,d3):
        self.data[0].append(d1*1000)
        self.data[1].append(d2*1000)
        self.data[2].append(d3*1000)

    def setFs(self,fs):
        self.fs = fs



##############################################################################
## main

        
qtPanningPlot = QtPanningPlot("Einthoven ECG I,II,III")

# The high- and lowpass filters can only be set after
# the sampling rate is known
hpiir1 = False
hpiir2 = False
bsiir1 = False
bsiir2 = False

# init the filters once we know the sampling rate
def callbackFs(fs):
    global hpiir1,hpiir2,bsiir1,bsiir2
    hpfc = 0.5 # highpass freq
    hpsos = signal.butter(2, hpfc, output='sos', btype='highpass', fs=fs)
    hpiir1 = iir_filter.IIR_filter(hpsos)
    hpiir2 = iir_filter.IIR_filter(hpsos)
    bssos = signal.butter(2, [fbs-1,fbs+2], output='sos', btype='bandstop', fs=fs)
    bsiir1 = iir_filter.IIR_filter(bssos)
    bsiir2 = iir_filter.IIR_filter(bssos)
    qtPanningPlot.setFs(fs)

def callbackSetChannel(c):
    global channel
    channel = c

# process data with the filters set up
def callbackData(data):
    global CH1,CH2
    v2 = hpiir1.filter(data[CH1])
    v2 = bsiir1.filter(v2)
    v3 = hpiir2.filter(data[CH2])
    v3 = bsiir2.filter(v3)
    qtPanningPlot.addData(v2-v3,v2,v3)

    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

# showing all the windows
pg.exec()

attysScopeReader.stop()

print("Finished")
