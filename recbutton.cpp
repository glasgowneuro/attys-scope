#include "recbutton.h"

RecButton::RecButton() : QPushButton() {
	setCheckable(true);
	connect(this, SIGNAL (toggled(bool)), this, SLOT (handleToggled(bool)));
	setText("Record");
}

void RecButton::handleToggled(bool) {
	recording = !recording;
	if (recording) {
		setStyleSheet("background-color: red");
	} else {
		setStyleSheet("background-color: rgb(64,64,64)");
	}
	emit stateChanged((int)recording);
}
