#pragma comment(lib, "Ws2_32.lib")

class ComediScope;
#ifndef COMEDISCOPE_H
#define COMEDISCOPE_H

#define MAXBUFFER 65536

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout> 
#include <QPaintEvent>
#include <QTimerEvent>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>

#include <Iir.h>

#include <fcntl.h>

#include <string>

#include "attys_scope.h"

#include "AttysComm.h"

#define MAX_DISP_X 4096 // max screen width

#define IIRORDER 2

#define VOLT_FORMAT_STRING "%+.3f"

class ComediScope : public QWidget
{
    Q_OBJECT
public:
/**
 * Constructor:
 **/
    ComediScope( Attys_scope* attys_scope_tmp,
		 float notchF = 50
	);
/**
 * Destructor: close the file if necessary
 **/
    ~ComediScope();


private:
    void btprintf(int s,const char *message,int checkOK);

protected:
/**
 * Overloads the empty paint-function: draws the functions and saves the data
 **/
    void	paintEvent( QPaintEvent * );

public:
/**
 * Clears the screen
 **/
    void        clearScreen();

private:
/**
 * Paints the data on the screen, is callsed by paintEvent
 **/
    void        paintData(float** buffer);

/**
 * Is called by the timer of the window. This causes the drawing and saving
 * of all not yet displayed/saved data.
 **/
    void	timerEvent( QTimerEvent * );

private slots:

/**
 * Updates the time-code and also the voltages of the channels
 **/
    void	updateTime();

private:
/**
 * Saves data which has arrived from the AD-converter
 **/
    void        writeFile();

private:
    /**
     * file descriptor for bt devices
     **/
    SOCKET *dev;

 private:
    unsigned int** chanlist;
    
 private:
    int subdevice;

    /**
     * y-positions of the data
     **/
    int         ***ypos;

    /**
     * current x-pos
     **/
    int         xpos;

    /**
     * elapsed msec
     **/
    long int         nsamples;

	AttysComm** attysComm;

public:
    /**
     * sets the filename for the data-file
     **/
    void        setFilename(QString name,int csv);

public:
    /**
     * sets the time between the samples
     **/
    void        setTB(int us);

private:
    /**
     * pointer to the parent widget which contains all the controls
     **/
    Attys_scope* attys_scope;
  

private:
    /**
     * the filename of the data-file
     **/
    QString*     rec_filename;

    /**
     * the file descriptor to the data-file
     **/
    FILE*       rec_file;

    /**
     * number of channels switched on
     **/
    int         num_channels;

private:
    /**
     * buffer which adds up the data for averaging
     **/
    float**   adAvgBuffer;

private:
    /**
     * init value for the averaging counter
     **/
    int         tb_init;

    int eraseFlag;

private:
    /**
     * counter for the tb. If zero the average is
     * taken from adAvgBuffer and saved into actualAD.
     **/
    int         tb_counter;

public:
    /**
     * Starts recording
     **/
    void startRec();

    /**
     * Ends recording
     **/
    void stopRec();

private:
    /**
     * the number of channels actually used per comedi device
     **/
    int channels_in_use;

public:
    int getNchannels() {return channels_in_use;};

private:
    /**
     * Frequency for the notch filter in Hz
     **/
    float notchFrequency;

public:
    /**
     * sets the notch frequency
     **/
    void setNotchFrequency(float f);

    /**
     * gets the notch frequency
     **/
    float getNotchFrequency() {
	    return notchFrequency;
    }

 private:
    /**
     * flag if data has been recorded. Prevents overwrite.
     **/
    int recorded;
    /**
     * the max value of the A/D converter
     **/
    int** maxdata;

    /**
     * notch filter
     **/
    Iir::Butterworth::BandStop<IIRORDER>*** iirnotch;

    /**
     * comma separated?
     **/
    char separator;

/**
 * Number of detected comedi devices
 **/
    int nComediDevices;

/**
 * Timer for printing the voltages in the textfields
 **/
    QTimer* counter;

/**
 * Raw daq data from the A/D converter which is saved to a file
 **/
    float** daqData;


public:
/**
 * Start the DAQ board(s)
 **/
    void startDAQ();

public:
/**
 * Gets the number of comedi devices actually used here
 **/
    int getNcomediDevices() {return nComediDevices;};


	public:
	int getActualSamplingRate() { return attysComm[0]->getSamplingRateInHz(); }

};



#endif
