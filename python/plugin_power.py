#!/usr/bin/python3
"""
Requires pyqtgraph.

Copyright (c) 2018-2022, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

Plots signal and power.

It's a demo how use the callback based approach and filtering.
Calculates the instantaneous power.
"""

channel = 7 # 1st ADC unfiltered

import sys
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui
import numpy as np
from scipy import signal
import iir_filter
from attys_scope_plugin_comm import AttysScopeReader

# create a global QT application object
app = QtGui.QApplication(sys.argv)


class QtPanningPlot:

    def __init__(self,title,miny,maxy):
        self.win = pg.GraphicsLayoutWidget()
        self.win.setWindowTitle(title)
        self.plt = self.win.addPlot()
        self.plt.setYRange(miny,maxy)
        self.plt.setXRange(0,500)
        self.curve = self.plt.plot()
        self.data = []
        # any additional initalisation code goes here (filters etc)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)
        self.layout = QtGui.QGridLayout()
        self.win.setLayout(self.layout)
        self.win.show()
        
    def update(self):
        self.data=self.data[-500:]
        if self.data:
            self.curve.setData(np.hstack(self.data))

    def addData(self,d):
        self.data.append(d)
        
qtPanningPlot1 = QtPanningPlot("Signal",-2,2)
qtPanningPlot2 = QtPanningPlot("Power",0,3)

# The high- and lowpass filters can only be set after
# the sampling rate is known
hpiir = False
lpiir = False

# init the filters once we know the sampling rate
def callbackFs(fs):
    global lpiir
    lpfc = 1 # lowpass freq
    lpsos = signal.butter(2, lpfc/fs*2, output='sos')
    lpiir = iir_filter.IIR_filter(lpsos)


# process data with the filters set up
def callbackData(data):
    v = data[channel]
    qtPanningPlot1.addData(v)
    v = v*v
    v = lpiir.filter(v)
    qtPanningPlot2.addData(v)

    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

# showing all the windows
app.exec_()

attysScopeReader.stop()

print("Finished")
