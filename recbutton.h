#ifndef _RECBUTTON
#define _RECBUTTON

#include <QPushButton>


class RecButton : public QPushButton {

	Q_OBJECT

public:
	RecButton();
	bool checkState() {
		return isChecked();
	}

private slots:
	void handleToggled(bool);

signals:
	void stateChanged(int i);

private:
	bool recording = false;
	QPalette* pal;
};

#endif
