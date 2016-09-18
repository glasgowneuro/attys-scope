#include "hp.h"

#include<qfont.h>
#include<stdio.h>

Hp::Hp(float samplingrate,float cutoff) : QCheckBox() {
	hp = new Iir::Butterworth::HighPass<HPORDER>;
	hp->setup (HPORDER, 
		   samplingrate, 
		   cutoff);
}

float Hp::filter (float v) {
	if (checkState()==Qt::Checked) {
		return hp->filter(v);
	} else {
		return v;
	}
}
