#include "acceleration.h"

#include<qfont.h>
#include<stdio.h>


Acceleration::Acceleration() : QComboBox() {
	addItem(tr(" 2g"),AttysComm::ACCEL_2G);
	addItem(tr(" 4g"),AttysComm::ACCEL_4G);
	addItem(tr(" 8g"),AttysComm::ACCEL_8G);
	addItem(tr("16g"),AttysComm::ACCEL_16G);

	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT ( accelerationChanged(int) ) );

	setAcceleration(AttysComm::ACCEL_16G);
}

void Acceleration::accelerationChanged ( int index ) {
 	acceleration = index;
	emit signalRestart();
}

void Acceleration::setAcceleration( int c ) {
	if (c < count()) {
		acceleration = c;
		setCurrentIndex(c);
	}
}

int Acceleration::getAccelerationIndex() {
	return accelerationMapping[acceleration];
}
