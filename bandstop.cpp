#include "bandstop.h"

#include<qfont.h>
#include<stdio.h>

Bandstop::Bandstop(float _samplingrate, float _center, float _width) : QComboBox() {
	frequency = _center;
	samplingrate = _samplingrate;
	if (_width > 0) {
		width = _width;
	}
	else {
		width = 5;
	}

	setMinimumWidth ( fontMetrics().width("10000Hz___") );

	addItem(tr("off"),0);
	addItem(tr("50Hz"), 50);
	addItem(tr("60Hz"), 60);

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( setFrequencyIndex(int) ) );

	setFrequency(_center);
}

void Bandstop::setFrequencyIndex ( int index ) {
 	frequency = (float)(itemData(index).toFloat());
	if (frequency > 0) {
		bs.setup(samplingrate,
			 frequency,
			 width);
		//_RPT2(0, "Bandstop=%fHz,idx=%d\n", frequency,index);
	}
}

void Bandstop::setFrequency(float f) {
	//_RPT1(0, "setFre Bandstop=%fHz\n", f);
	int index = findData(f);
	if (index != -1) {
		setCurrentIndex(index);
		setFrequencyIndex(index);
	}
}

float Bandstop::filter(float v) {
	if (frequency > 0) {
		return bs.filter(v);
	}
	return v;
}
