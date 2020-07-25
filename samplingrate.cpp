#include "samplingrate.h"

#include<qfont.h>
#include<stdio.h>


SamplingRate::SamplingRate() : QComboBox() {
	addItem(tr("125Hz"),SAMPLING_RATE_125HZ);
	addItem(tr("250Hz"),SAMPLING_RATE_250HZ);
	addItem(tr("500Hz"),SAMPLING_RATE_500HZ);

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( samplingRateChanged(int) ) );

	setSamplingRate(AttysComm::ADC_RATE_250HZ);
}

void SamplingRate::samplingRateChanged ( int index ) {
 	samplingRate = index;
	emit signalRestart();
}

void SamplingRate::setSamplingRate( int c ) {
		samplingRate = c;
		setCurrentIndex(c);
}

int SamplingRate::getSamplingRateIndex() {
	if ((samplingRate < 0) || (samplingRate > SAMPLING_RATE_500HZ)) return AttysComm::ADC_RATE_250HZ;
	return samplingRateMapping[samplingRate];
}
