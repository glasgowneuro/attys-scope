#include "highpass.h"

#include<qfont.h>
#include<stdio.h>


Highpass::Highpass(float _samplingrate, float cutoff) : QComboBox() {

	hp = new Iir::Butterworth::HighPass<HPORDER>;
	frequency = cutoff;
	samplingrate = _samplingrate;
	dcValue = 0;

	if (cutoff > 0) {
		hp->setup(HPORDER,
			_samplingrate,
			cutoff);
	}

	setMinimumWidth ( fontMetrics().width("1000Hz") );

	addItem(tr("off"),-1);
	addItem(tr("-DC"),0);
	addItem(tr("0.1Hz"),1);
	addItem(tr("0.2Hz"),2);
	addItem(tr("0.5Hz"),5);
	addItem(tr("1Hz"),10);
	addItem(tr("2Hz"), 20);
	addItem(tr("5Hz"), 50);
	addItem(tr("10Hz"),100);

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( setFrequency(int) ) );

}

void Highpass::setFrequency ( int index ) {
 	frequency = itemData(index).toInt()/10.0;
	if (frequency > 0) {
		hp->setup(HPORDER,
			samplingrate,
			(float)frequency);
		_RPT1(0, "%f\n", frequency);
		hp->reset();
	}
}

float Highpass::filter(float v) {
	if (frequency == 0) {
		v = v - dcValue;
		return v;
	}
	dcValue = dcValue + (v - dcValue) / INERTIA_FOR_DC_DETECTION;
	if (frequency > 0) {
		return hp->filter(v);
	}
	return v;
}
