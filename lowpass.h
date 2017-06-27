#ifndef _LOWPASS
#define _LOWPASS

#include <QComboBox>

#include <Iir.h>

#define LPORDER 2

class Lowpass : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
Lowpass(float _samplingrate, float cutoff = 0);

private slots:
    void setFrequency(int i);

private:
    float frequency;
	float samplingrate;

public:
    inline int getFrequency() {return frequency;};

private:
	Iir::Butterworth::LowPass<LPORDER>* lp;

public:
	float filter(float v);

};

#endif
