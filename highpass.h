#ifndef _HIGHPASS
#define _HIGHPASS

#include <QComboBox>

#include <Iir.h>

#define HPORDER 4

// defines how quickly the DC detector follows the signal
// the larger the value the slower
#define INERTIA_FOR_DC_DETECTION 1000

class Highpass : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
Highpass(float _samplingrate, float cutoff = -1);

private slots:
    void setFrequencyIndex(int i);

private:
    float frequency;
	float samplingrate;
	int dcCtr;

public:
    inline float getFrequency() {return frequency;};

	void setFrequency(float f);

private:

private:
	float dcValue;

private:
	Iir::Butterworth::HighPass<HPORDER> hp;

public:
	float filter(float v);

};

#endif
