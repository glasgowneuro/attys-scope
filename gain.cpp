#include "gain.h"

#include<qfont.h>
#include<stdio.h>


Gain::Gain() : QComboBox() {

	setMinimumWidth ( fontMetrics().width("_x5000XX_") );

	addItem(tr("x1"),1.0);
	addItem(tr("x2"),2.0);
	addItem(tr("x5"),5.0);
	addItem(tr("x10"),10.0);
	addItem(tr("x20"),20.0);
	addItem(tr("x50"),50.0);
	addItem(tr("x100"),100.0);
	addItem(tr("x200"),200.0);
	addItem(tr("x500"),500.0);
	addItem(tr("x1000"), 1000.0);
	addItem(tr("x2000"), 2000.0);
	addItem(tr("x5000"), 5000.0);
	gain = 1;

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( setGainIndex(int) ) );

}

void Gain::setGainIndex ( int index ) {
 	gain = itemData(index).toInt();
}

void Gain::setGain(float g) {
	int index = findData(g);
	if (index != -1) {
		setCurrentIndex(index);
		setGainIndex(index);
	}
}
