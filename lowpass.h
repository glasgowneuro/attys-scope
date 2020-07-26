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
Lowpass();

private slots:
    void setFrequencyIndex(int i);

private:
    float frequency = 0;
    float samplingrate = 250;

public:
    inline float getFrequency() {return frequency;};
    void setFrequency(float f);
    void setSamplingRate(float r) { samplingrate = r; };

private:
    Iir::Butterworth::LowPass<LPORDER> lp;

public:
    float filter(float v);

};

#endif
