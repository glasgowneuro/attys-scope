#ifndef _SAMPLING_RATE
#define _SAMPLING_RATE

#include<AttysComm.h>

#include <QComboBox>

#define SAMPLING_RATE_125HZ 0
#define SAMPLING_RATE_250HZ 1
#define SAMPLING_RATE_500HZ 2

class SamplingRate : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
	SamplingRate(int highSpeed);

	const int samplingRateMapping[3] = {
		AttysComm::ADC_RATE_125HZ,
		AttysComm::ADC_RATE_250HZ,
		AttysComm::ADC_RATE_500HZ
	};

private slots:
	void samplingRateChanged(int i);

signals:
	void signalRestart();

private:
	int samplingRate = AttysCommBase::ADC_RATE_250HZ;

	int isHighSpeed = 0;
	
public:
	inline int getSamplingRate() {return samplingRate;};

	int getSamplingRateIndex();

	void setSamplingRate(int _sampingRate);
};

#endif
