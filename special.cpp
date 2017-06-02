#include "special.h"

#include<qfont.h>
#include<stdio.h>


Special::Special() : QComboBox() {

	setMinimumWidth ( fontMetrics().width("x50XXg") );

	addItem(tr("normal"),SPECIAL_NORMAL);
	addItem(tr("ECG"),SPECIAL_ECG);
	addItem(tr("I const"), SPECIAL_I);

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( specialChanged(int) ) );

}

void Special::specialChanged ( int index ) {
 	special = index;
	_RPT1(0, "special %d\n",special);
}

void Special::setSpecial( int c ) {
		special = c;
		setCurrentIndex(c);
}

