#ifndef _BANDSTOP
#define _BANDSTOP

#include <QComboBox>

#include <Iir.h>

#define IIRORDER 2

class Bandstop : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
    Bandstop();

private slots:
    void setFrequencyIndex(int i);

private:
    float frequency = 0;
    float samplingrate = 250;
    float width = 5;

public:
    inline float getFrequency() {return frequency;};
    void setFrequency(float f);
    void setSamplingRate(float r) {samplingrate = r;}; 

private:
    Iir::Butterworth::BandStop<IIRORDER> bs;

public:
    float filter(float v);

};

#endif
