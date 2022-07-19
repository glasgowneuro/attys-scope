import threading
import sys
import numpy as np

class AttysScopeReader:
    # Constructor needs a callback for the data and optionally for the sampling rate
    def __init__(self,hasDataCallback,hasFs=False):
        self.callback = hasDataCallback
        self.fsCallback = hasFs
        self.t = threading.Thread(target=self.readstdin)
        self.fs = False
        self.t2 = False

    # This reads the data from the socket in an endless loop
    # and calls the callback
    def readstdin(self):
        self.doRun = True
        while self.doRun:
            data = sys.stdin.readline()
            values = np.array(data.split(','),dtype=np.float32)
            if (self.t2) and (not self.fs):
                # print(values[0],self.t2)
                self.fs = 1/(values[0] - self.t2)
                self.fs = round(self.fs/125)*125
                print("fs =",self.fs)
                if self.fsCallback:
                    self.fsCallback(self.fs)
            self.t2 = values[0]
            if self.fs:
                self.callback(values)

    # Starts reading from attys-scope
    def start(self):
        self.t.start()

    # Stops reading from attys-scope
    def stop(self):
        self.doRun = False
        self.t.join()
