#ifndef _LP
#define _LP

#include <QCheckBox>
#include <Iir.h>

#define LPORDER 4

class Lp : public QCheckBox {

    Q_OBJECT

	    public:
	/**
	 * constructor
	 **/
	Lp(float samplingrate, float cutoff = 1);
	
private:
	Iir::Butterworth::LowPass<LPORDER>* lp;
	
public:
	float filter(float v);
	
};

#endif
