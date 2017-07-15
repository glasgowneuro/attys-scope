import numpy as np
from scipy import signal as signal

"""
 ECG R-peak detector and heart rate detector

 The detector uses the matched filter approach by creating
 an IIR bandpass filter which looks like an R peak and
 thus is a recursive matched filter. One could also say
 it's a causal wavelet or perhaps just a bandpass filter
 which covers the frequency range of the R peak. It's all
 the same in different forms!
 As an input the detector just gets the data samples
 at a given sampling rate and then it detects the r-peak and
 heart rate from it.
 It also has a callback function which is called when
 a heartbeat is detected (implemented as a listener).
"""


class heartrate_detector:

    def __init__(self,_fs):
        # how fast the adaptive threshold follows changes in ECG
        # amplitude. Realisic values: 0.1 .. 1.0
        # 0.1 = slow recovery after an artefact but no wrong detections
        # 1 = fast recovery after an artefact but possibly wrong detections
        self.adaptive_threshold_decay_constant = 0.25

        # the threshold for the detection is 0.6 times smaller than the amplitude
        self.threshold_factor = 0.6

        # 0.5mV as the thereshold the bandpass filtered ECG is an artefact
        self.artefact_threshold = 1

        # ignores 1000 samples to let the filter settle
        self.ignoreECGdetector = 1000

        #adaptive amplitude value of the detector output
        self.amplitude = 0.0

        def getAmplitude(self):
            return amplitude
        self.timestamp = 0

        # previous timestamp
        self.t2 = 0

        # timewindow not to detect an R peak
        self.doNotDetect = 0

        self.ignoreRRvalue = 2

        # create a 2nd order order bandpass filter
        center = 20
        width = 15
        f1 = center-width/2
        f2 = center+width/2
        self.bp_b, self.bp_a = signal.butter(2, [f1/_fs*2.0, f2/_fs*2.0 ], 'bandpass')
        # create the memory of the filter (=delay lines) so that
        # incoming data can be shifted through sample by sample
        self.bp_z = signal.lfiltic(self.bp_b, self.bp_a, [0])

        # sampling rate in Hz
        self.samplingRateInHz = _fs

        # heartrate in BPM
        self.bpm = 0;


    # detect r peaks
    # input: ECG samples at the specified sampling rate and in V
    def detect(self,v):
        #print('v=',v)
        h,self.bp_z = signal.lfilter(self.bp_b, self.bp_a, [v], zi = self.bp_z)
        if (self.ignoreECGdetector > 0):
            self.ignoreECGdetector = self.ignoreECGdetector - 1
            return
        h = h * h
        if (np.sqrt(h) > self.artefact_threshold):
            # ignore signal for 1 sec
            ignoreECGdetector = samplingRateInHz;
            ignoreRRvalue = 2
            return
        if (h > self.amplitude):
            self.amplitude = h
            
        self.amplitude = self.amplitude - self.adaptive_threshold_decay_constant * self.amplitude / self.samplingRateInHz

        if (self.doNotDetect > 0):
            self.doNotDetect = self.doNotDetect - 1
        else:
            self.threshold = self.threshold_factor * self.amplitude
            if (h > self.threshold):
                t = (self.timestamp - self.t2) / self.samplingRateInHz;
                if t>0:
                    tbpm = 1 / t * 60
                else:
                    tbpm = 0
                if ((tbpm > 30) and (tbpm < 250)):
                    if (self.ignoreRRvalue > 0):
                        self.ignoreRRvalue = self.ignoreRRvalue - 1
                    else:
                        self.bpm = tbpm
                        print(self.bpm," BPM")
                else:
                    self.ignoreRRvalue = 3
                self.t2 = self.timestamp
                # advoid 1/5 sec
                self.doNotDetect = self.samplingRateInHz / 5;
        self.timestamp = self.timestamp + 1
