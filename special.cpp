#include "special.h"

#include<qfont.h>
#include<stdio.h>


Special::Special() : QComboBox() {
	addItem(tr("normal"),SPECIAL_NORMAL);
	addItem(tr("ECG/Einth"),SPECIAL_ECG);
	addItem(tr("I const"), SPECIAL_I);
	addItem(tr("Temperature"), SPECIAL_TEMPERATURE);
	addItem(tr("PGA:x6"), SPECIALADC_GAIN_6);
	addItem(tr("PGA:x2"), SPECIALADC_GAIN_2);
	addItem(tr("PGA:x3"), SPECIALADC_GAIN_3);
	addItem(tr("PGA:x4"), SPECIALADC_GAIN_4);
	addItem(tr("PGA:x8"), SPECIALADC_GAIN_8);
	addItem(tr("PGA:x12"), SPECIALADC_GAIN_12);

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( specialChanged(int) ) );

	setSpecial(0);
}

void Special::specialChanged ( int index ) {
 	special = index;
	emit signalRestart();
//	_RPT1(0, "special %d\n",special);
}

void Special::setSpecial( int c ) {
		special = c;
		setCurrentIndex(c);
}

int Special::getGainIndex() {
	if ((special < 0) || (special > SPECIALADC_GAIN_12)) return AttysComm::ADC_GAIN_1;
	return gainMapping[special];
}
