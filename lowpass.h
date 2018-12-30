#ifndef _LOWPASS
#define _LOWPASS

#include <QComboBox>

#include <Iir.h>

#define LPORDER 4

class Lowpass : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
Lowpass(float _samplingrate, float cutoff = 0);

private slots:
    void setFrequencyIndex(int i);

private:
    float frequency;
	float samplingrate;

public:
    inline float getFrequency() {return frequency;};

	void setFrequency(float f);

private:
	Iir::Butterworth::LowPass<LPORDER> lp;

public:
	float filter(float v);

};

#endif
