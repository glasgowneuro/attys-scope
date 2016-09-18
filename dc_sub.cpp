#include "dc_sub.h"

#include<qfont.h>
#include<stdio.h>

DCSub::DCSub(float inertia) : QCheckBox() {

	dcValue = 0;
	divisor = inertia;

}

float DCSub::filter (float v) {
	if (checkState()==Qt::Checked) {
		v = v - dcValue;
	} else {
		dcValue = dcValue + (v - dcValue)/divisor;
	}
	return v;
}
