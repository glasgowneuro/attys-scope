# plots all ECG channels
# recorded with the Attys: www.attys.tech
#
import numpy as np
import pylab as pl
import scipy.signal as signal
#
data = np.loadtxt('mydata.tsv');
#
fig = pl.figure(1)
#
pl.title('ADC data measured with the Attys');

f1 = 45
f2 = 55
fs = 250
b1,a1 = signal.butter(2, [f1/fs*2,f2/fs*2], 'stop')
b2,a2 = signal.butter(2, 0.75/fs*2, 'high')

ch1 = data[:,7]
ch1 = signal.lfilter(b1,a1,ch1)
ch1 = signal.lfilter(b2,a2,ch1)
pl.subplot(211);
pl.plot(data[:,0],ch1);
pl.xlabel('time/sec');
pl.ylabel('Ch1/V');
pl.xlim(2,);


ch2 = data[:,8]
ch2 = signal.lfilter(b1,a1,ch2)
ch2 = signal.lfilter(b2,a2,ch2)
pl.subplot(212);
pl.plot(data[:,0],ch2);
pl.xlabel('time/sec');
pl.ylabel('Ch2/V');
pl.xlim(2,);

pl.figure(2)

ch1 = ch1[fs*2:]
ch2 = ch2[fs*2:]
pl.subplot(211);
pl.plot(np.linspace(0,250,len(ch1)),np.abs(np.fft.fft(ch1)));
pl.xlabel('f');
pl.ylabel('ampl');

pl.subplot(212);
pl.plot(np.linspace(0,250,len(ch2)),np.abs(np.fft.fft(ch2)));
pl.xlabel('f');
pl.ylabel('ampl');
pl.show();
