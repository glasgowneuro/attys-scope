#include "lowpass.h"

#include<qfont.h>
#include<stdio.h>


Lowpass::Lowpass(float _samplingrate, float cutoff) : QComboBox() {

	lp = new Iir::Butterworth::LowPass<LPORDER>;
	frequency = cutoff;
	samplingrate = _samplingrate;
	if (cutoff > 0) {
		lp->setup(LPORDER,
			_samplingrate,
			cutoff);
	}

	setMinimumWidth ( fontMetrics().width("10000Hz") );

	addItem(tr("off"),0);
	addItem(tr("1Hz"), 1);
	addItem(tr("2Hz"), 2);
	addItem(tr("5Hz"), 5);
	addItem(tr("10Hz"),10);
	addItem(tr("20Hz"), 20);
	addItem(tr("50Hz"), 50);
	addItem(tr("||1Hz"), -1);
	addItem(tr("||2Hz"), -2);
	addItem(tr("||5Hz"), -5);

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( setFrequency(int) ) );

}

void Lowpass::setFrequency ( int index ) {
 	frequency = (float)(itemData(index).toInt());
	if (fabs(frequency) > 0) {
		lp->setup(LPORDER,
			samplingrate,
			(float)fabs(frequency));
		_RPT1(0, "lowpass=%f\n", frequency);
		lp->reset();
	}
}

float Lowpass::filter(float v) {
	if (frequency > 0) {
		return lp->filter(v);
	}
	else {
		if (frequency < 0) {
			return lp->filter(fabs(v));
		}
	}
	return v;
}
