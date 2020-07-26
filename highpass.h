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
Highpass();

private slots:
    void setFrequencyIndex(int i);

private:
    float frequency = 0;
    float samplingrate = 250;
    int dcCtr = 10;

public:
    inline float getFrequency() {return frequency;};
	
    void setFrequency(float f);

    void setSamplingRate(float r) {
	    samplingrate = r;
	    dcCtr = r*10;
    };

private:

private:
	float dcValue;

private:
	Iir::Butterworth::HighPass<HPORDER> hp;

public:
	float filter(float v);

};

#endif
