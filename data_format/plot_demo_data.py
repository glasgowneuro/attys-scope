# plots all ECG channels
# recorded with the Attys: www.attys.tech
#
import numpy as np
import pylab as pl
#
data = np.loadtxt('leiden_data_test00.tsv');
#
fig = pl.figure(1)
#
pl.title('Leiden data. Random selection of a couple of channels.');
# I
pl.subplot(411);
pl.plot(data[:,0],data[:,7]);
pl.xlabel('time/sec');
pl.ylabel('Attys#1, ADC Ch1/V');
#pl.ylim(-2,2);
# II
pl.subplot(412);
pl.plot(data[:,0],data[:,8]);
pl.xlabel('time/sec');
pl.ylabel('Attys#1, ADC Ch2/V');
#pl.ylim(0,1);
# I
pl.subplot(413);
pl.plot(data[:,0],data[:,17]);
pl.xlabel('time/sec');
pl.ylabel('Attys#2 ADC Ch1/V');
#pl.ylim(-2,2);
# II
pl.subplot(414);
pl.plot(data[:,0],data[:,18]);
pl.xlabel('time/sec');
pl.ylabel('Attys#2, ADC Ch2/V');
#pl.ylim(0,1);
pl.show();
