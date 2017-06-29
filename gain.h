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
    void setGainIndex(int i);

private:
    float gain;

public:
    float getGain() {return gain;};

	void setGain(float g);
};

#endif
