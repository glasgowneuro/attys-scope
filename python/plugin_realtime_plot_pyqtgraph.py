#!/usr/bin/python3
"""
Requires pyqtgraph.

Copyright (c) 2018-2022, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

Realtime dataplot with myqtgraph and DC removal
"""

channel = 7 # 1st ADC unfiltered

import sys
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtWidgets
import numpy as np
from attys_scope_plugin_comm import AttysScopeReader

app = pg.mkQApp()

class QtPanningPlot:

    n = 500

    def __init__(self,title):
        self.title = title
        self.channelCallback = False
        self.mw = QtWidgets.QMainWindow()
        self.mw.setWindowTitle('Dataplot')
        self.mw.resize(800,600)
        self.cw = QtWidgets.QWidget()
        self.mw.setCentralWidget(self.cw)
        self.l = QtWidgets.QVBoxLayout()
        self.cw.setLayout(self.l)
        self.pw = pg.PlotWidget()
        self.l.addWidget(self.pw)
        self.pw.setLabel('bottom', 't/sec')
        self.pw.setYRange(-1.5,1.5)
        self.plt = self.pw.plot()

        self.data = []
        # any additional initalisation code goes here (filters etc)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)
        self.mw.show()

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


##############################################################################
## main

        
qtPanningPlot = QtPanningPlot("EMG amplitude")

# init the filters once we know the sampling rate
def callbackFs(fs):
    qtPanningPlot.setFs(fs)

# process data with the filters set up
def callbackData(data):
    v = data[channel]
    qtPanningPlot.addData(v)

    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

# showing all the windows
pg.exec()

attysScopeReader.stop()

print("Finished")
