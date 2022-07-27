#ifndef _ACCELERATION
#define _ACCELERATION

#include<AttysComm.h>

#include <QComboBox>

class Acceleration : public QComboBox {

    Q_OBJECT

public:
    /**
     * constructor
     **/
	Acceleration();

	const int accelerationMapping[4] = {
		AttysCommBase::ACCEL_2G,
		AttysCommBase::ACCEL_4G,
		AttysCommBase::ACCEL_8G,
		AttysCommBase::ACCEL_16G
	};

	/**
         * One g in m/s^2.
         **/
        const float oneG = 9.80665F; // m/s^2
	
        /**
         * Mapping of the index to the full scale accelerations.
         **/
        const float ACCEL_FULL_SCALE[4] = { 2 * oneG, 4 * oneG, 8 * oneG, 16 * oneG }; // m/s^2

private slots:
	void accelerationChanged(int i);

signals:
	void signalRestart();

private:
	int acceleration = AttysCommBase::ACCEL_16G;
	
public:
	inline int getAcceleration() {return acceleration;};

	int getAccelerationIndex();

	void setAcceleration(int _sampingRate);

	int getAccelerationInmss() {
		return ACCEL_FULL_SCALE[acceleration];
	}
};

#endif
