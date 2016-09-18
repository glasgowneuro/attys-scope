#ifndef _GAIN
#define _GAIN

#include <QComboBox>

class Gain : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
    Gain();

private slots:
    void setGain(int i);

private:
    int gain;

public:
    inline float getGain() {return gain;};

};

#endif
