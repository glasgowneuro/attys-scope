class Attys_scope;
#ifndef ATTYS_SCOPE_H
#define ATTYS_SCOPE_H

#include <QWidget>
#include <QPainter>
#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout> 
#include <QTextEdit>
#include <QGroupBox>
#include <QLabel>
#include <qsplashscreen.h>

#include "comediscope.h"
#include "gain.h"
#include "channel.h"
#include "dc_sub.h"
#include "hp.h"
#include "lp.h"

// defines how quickly the DC detector follows the signal
// the larger the value the slower
#define INERTIA_FOR_DC_DETECTION 1000

class Attys_scope : public QWidget
{

    Q_OBJECT
public:
/**
 * Constructor
 **/
	Attys_scope( QWidget *parent,
		      QSplashScreen *_splash,
		      int ignoreSettings = 0,
		      float notch = 50.0,
		      int num_of_devices = 1,
		      int csv = 0,
		      float lpFreq = 10,
		      float hpFreq = 1
		);
	
	/**
	 * Destructor
	 **/
	~Attys_scope();
	
public:
/**
 * Sets the filename for the data-file
 **/
	void setFilename(QString fn,int csv);

public:
	void enableControls();
	void disableControls();

public:

	QSplashScreen *splash;

/**
 * Button which controls recording
 **/
    QCheckBox *recPushButton;

/**
 * channel label
 **/
    QLabel*** channelLabel;

    QLabel*** subDClabel;

    QLabel*** hpLabel;

    QLabel*** lpLabel;

    /**
     * Notch filter on?
     **/
    QCheckBox* filterCheckbox;

public:
    /**
     * Comments
     **/
    QTextEdit* commentTextEdit;

    /**
     * Shows the filter frequency
     **/
    QTextEdit*  filterTextEdit;

    /**
     * Array of the voltages
     **/
    QTextEdit***  voltageTextEdit;

    /**
     * Array for the gain settings
     **/
    Channel*** channel;

    /**
     * Array for the gain settings
     **/
    Gain*** gain;

    /**
     * subtracting DC
     **/
    DCSub*** dcSub;

    /**
     * highpass filter
     **/
    Hp*** hp;

    /**
     * lowpass filter
     **/
    Lp*** lp;

    /**
     * The widget which contains the graphical plots of the AD-data
     **/
    ComediScope* comediScope;

    /**
     * Text-field: elapsed time
     **/
    QTextEdit* timeInfoTextEdit;

    /**
     * Text-field for the file-name
     **/
    QPushButton *filePushButton;

 private:
    /**
     * Button: Increase the time between samples
     **/
    QPushButton *tbIncPushButton;

    /**
     * Button: Decrease the time between samples
     **/
    QPushButton *tbDecPushButton;

    /**
     * Button: Reset screen
     **/
    QPushButton *tbResetPushButton;

/**
 * Text-field: time between samples
 **/
    QTextEdit   *tbInfoTextEdit;

/**
 * Button: clears the screen
 **/
    QPushButton *clearScreenPushButton;

private slots:
/**
 * Button to increase the time-base has been pressed
 **/
    void incTbEvent();

/**
 * Reset screen
 **/
    void resetTbEvent(); 

/**
 * Button to decrease the time-base has been pressed
 **/
    void decTbEvent();
    
    /**
     * Enter a new filename
     **/
    void enterFileName();

    /**
     * Pressing or releasing the record button
     **/
    void recstartstop(int);

private:
/**
 * Called if a change in the time-base has occurred
 **/
    void changeTB();

/**
 * returns the timebase
 **/
public:
    int getTB() {return tb_us;};

private:
/**
 * Time between two samples in ms
 **/
    int tb_us;

/**
 * Group which keeps one channel together
 **/
    QGroupBox*** channelgrp;

/**
 * Layout which keeps one channel together
 **/
    QHBoxLayout*** hbox;

    QLabel *statusLabel;

};


#endif
