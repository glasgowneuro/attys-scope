#ifndef _SPECIAL
#define _SPECIAL

#include<AttysComm.h>

#include <QComboBox>

#define SPECIAL_NORMAL 0
#define SPECIAL_ECG 1
#define SPECIAL_I 2
#define SPECIAL_TEMPERATURE 3
#define SPECIALADC_GAIN_6 4
#define SPECIALADC_GAIN_2 5
#define SPECIALADC_GAIN_3 6
#define SPECIALADC_GAIN_4 7
#define SPECIALADC_GAIN_8 8
#define SPECIALADC_GAIN_12 9

class Special : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
	Special();

	const int gainMapping[1+SPECIALADC_GAIN_12] = {
		AttysComm::ADC_GAIN_1, // for NORMAL
		AttysComm::ADC_GAIN_6, // for ECG
		AttysComm::ADC_GAIN_1, // for I
		AttysComm::ADC_GAIN_1, // for TEMPERATURE
		AttysComm::ADC_GAIN_6, // for PGA=6
		AttysComm::ADC_GAIN_2, // for PGA=2
		AttysComm::ADC_GAIN_3, // for PGA=3
		AttysComm::ADC_GAIN_4, // for PGA=4
		AttysComm::ADC_GAIN_8, // for PGA=8
		AttysComm::ADC_GAIN_12 // for PGA=12
	};

private slots:
	void specialChanged(int i);

signals:
	void signalRestart();

private:
	int special;
	std::string* specialLabels;
	
public:
	inline int getSpecial() {return special;};

	int getGainIndex();

	void setSpecial(int _special);
};

#endif
