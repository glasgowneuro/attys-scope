# plots all ECG channels
# recorded with the Attys: www.attys.tech
#
import numpy as np
import pylab as pl
#
data = np.loadtxt('demodata.tsv');
#
fig = pl.figure(1)
#
pl.title('Demo data measured with the Attys');
# I
pl.subplot(211);
pl.plot(data[:,0],data[:,9]);
pl.xlabel('time/sec');
pl.ylabel('Ch1/V');
pl.ylim(-2,2);
# II
pl.subplot(212);
pl.plot(data[:,0],data[:,10]);
pl.xlabel('time/sec');
pl.ylabel('Ch2/V');
pl.ylim(0,1);
pl.show();
