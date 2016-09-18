#ifndef _HP
#define _HP

#include <QCheckBox>
#include <Iir.h>

#define HPORDER 4

class Hp : public QCheckBox {

    Q_OBJECT

	    public:
	/**
	 * constructor
	 **/
	Hp(float samplingrate, float cutoff = 1);
	
private:
	Iir::Butterworth::HighPass<HPORDER>* hp;
	
public:
	float filter(float v);
	
};

#endif
