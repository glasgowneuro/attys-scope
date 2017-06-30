#ifndef _BANDSTOP
#define _BANDSTOP

#include <QComboBox>

#include <Iir.h>

#define BSORDER 2

class Bandstop : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
Bandstop(float _samplingrate, float _center, float _width = 5);

private slots:
    void setFrequencyIndex(int i);

private:
    float frequency;
	float samplingrate;
	float width;

public:
    inline float getFrequency() {return frequency;};

	void setFrequency(float f);

private:
	Iir::Butterworth::BandStop<BSORDER>* bs;

public:
	float filter(float v);

};

#endif
