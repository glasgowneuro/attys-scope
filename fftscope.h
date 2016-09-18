class FFTScope;
#ifndef FFTSCOPE_H
#define FFTSCOPE_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h> 
#include <qtimer.h>

#include <fcntl.h>
#include <fftw3.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <QMainWindow>

#include "attys_scope.h"

#define FFT_UPDATE_DELAY 1000

#define FFT_BUFFER_SIZE 1024

class FFTScope : public QMainWindow
{
    Q_OBJECT
public:
/**
 * Constructor:
 **/
	FFTScope( Attys_scope* parent , 
		  int fft_buffer_size = FFT_BUFFER_SIZE);

	~FFTScope();

public:
	int append(double);

	int getBufferSize() {return  buffer_size;};

private:
	int filled;
	int current_entry;
	int buffer_size;
	QTimer *counter;

private slots:

/**
 * Updates the time-code and also the voltages of the channels
 **/
	void	updateFFT();
	
private:
	int 	rate;
	double *in;
	fftw_complex *out;
	QwtPlot *fftPlot;
	QwtPlotCurve *fftCurve;
	Attys_scope *attys_scope;
	double *x;
	double *y;
	int nFreqSamples;

};


#endif
