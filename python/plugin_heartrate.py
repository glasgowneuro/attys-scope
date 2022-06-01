#!/usr/bin/python3
"""
Requires pyqtgraph.

Copyright (c) 2018-2022, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

Plots heartrate

It's a demo how use the callback based approach and filtering.
"""

channel = 7 # 1st ADC unfiltered

import time
import sys
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui
import numpy as np
from scipy import signal
import iir_filter
from attys_scope_plugin_comm import AttysScopeReader
import ecg_analysis
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *


# create a global QT application object
app = QtGui.QApplication(sys.argv)


class QtPanningPlot:

    def __init__(self,title):
        self.win = pg.GraphicsLayoutWidget()
        self.win.setWindowTitle(title)
        self.plt = self.win.addPlot()
        self.plt.setYRange(0,200)
        self.plt.setXRange(0,60)
        self.curve = self.plt.plot()
        self.data = []
        # any additional initalisation code goes here (filters etc)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)
        self.layout = QtGui.QGridLayout()
        self.win.setLayout(self.layout)
        proxy1 = QtGui.QGraphicsProxyWidget()
        self.qlabelHR = QLabel()
        self.qlabelHR.setStyleSheet("background-color:black; color:white; font-size: 48px;");
        self.qlabelHR.setAlignment(Qt.AlignCenter);
        proxy1.setWidget(self.qlabelHR)
        p = self.win.addLayout()
        p.addItem(proxy1)
        self.win.show()
        
    def update(self):
        self.data=self.data[-60:]
        if self.data:
            self.curve.setData(np.hstack(self.data))

    def addData(self,d):
        self.qlabelHR.setText("{:10.1f} BPM".format(d))
        self.data.append(d)
        
qtPanningPlot1 = QtPanningPlot("Heartrate")

# will be properly set in the callback
heartrate_detector = False

def hasHR(bpm):
    qtPanningPlot1.addData(bpm)
    print(time.time(),bpm,flush=True)

# init the detector once we know the sampling rate
def callbackFs(fs):
    global heartrate_detector
    heartrate_detector = ecg_analysis.heartrate_detector(fs,hasHR)

# process data with the filters set up
def callbackData(data):
    v = data[channel]
    heartrate_detector.detect(v)
    
attysScopeReader = AttysScopeReader(callbackData,callbackFs)
attysScopeReader.start()

# showing all the windows
app.exec_()

attysScopeReader.stop()

print("Finished")
