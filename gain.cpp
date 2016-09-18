#include "gain.h"

#include<qfont.h>
#include<stdio.h>


Gain::Gain() : QComboBox() {

	setMinimumWidth ( fontMetrics().width("x50XX") );

	addItem(tr("x1"),1);
	addItem(tr("x2"),2);
	addItem(tr("x5"),5);
	addItem(tr("x10"),10);
	addItem(tr("x20"),20);
	addItem(tr("x50"),50);
	addItem(tr("x100"),100);
	addItem(tr("x200"),200);
	addItem(tr("x500"),500);
	gain = 1;

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( setGain(int) ) );

}

void Gain::setGain ( int index ) {
 	gain = itemData(index).toInt();
}
