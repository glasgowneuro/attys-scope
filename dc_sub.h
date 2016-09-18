#ifndef _DCSUB
#define _DCSUB

#include <QCheckBox>

class DCSub : public QCheckBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
    DCSub(float intertia);

private:
    float dcValue;
    float divisor;

public:
    float filter(float v);

};

#endif
