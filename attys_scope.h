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
#include <QScrollArea>
#include <qsplashscreen.h>
#include <qpointer.h>
#include <qfont.h>

#include "scopewindow.h"
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

	QPointer<QSplashScreen> splash;

/**
 * Button which controls recording
 **/
    QPointer<QCheckBox> recPushButton;

/**
 * channel label
 **/
	QPointer<QLabel>** channelLabel;

	QPointer<QLabel>** subDClabel;

	QPointer<QLabel>** hpLabel;

	QPointer<QLabel>** lpLabel;

    /**
     * Notch filter on?
     **/
	QPointer<QCheckBox> filterCheckbox;

public:
    /**
     * Comments
     **/
	QPointer<QTextEdit> notchTextEdit;

    /**
     * Shows the filter frequency
     **/
	QPointer<QTextEdit>  filterTextEdit;

    /**
     * Array of the voltages
     **/
	QPointer<QTextEdit>**  voltageTextEdit;

    /**
     * Array for the gain settings
     **/
	QPointer<Channel>** channel;

    /**
     * Array for the gain settings
     **/
	QPointer<Gain>** gain;

    /**
     * subtracting DC
     **/
	QPointer<DCSub>** dcSub;

    /**
     * highpass filter
     **/
	QPointer<Hp>** hp;

    /**
     * lowpass filter
     **/
	QPointer<Lp>** lp;

    /**
     * The widget which contains the graphical plots of the AD-data
     **/
	QPointer<ScopeWindow> attysScopeWindow;

    /**
     * Text-field: elapsed time
     **/
	QPointer<QTextEdit> timeInfoTextEdit;

    /**
     * Text-field for the file-name
     **/
	QPointer<QPushButton> filePushButton;

 private:
    /**
     * Button: Increase the time between samples
     **/
	 QPointer<QPushButton> tbIncPushButton;

    /**
     * Button: Decrease the time between samples
     **/
	 QPointer<QPushButton> tbDecPushButton;

    /**
     * Button: Reset screen
     **/
	 QPointer<QPushButton> tbResetPushButton;

/**
 * Text-field: time between samples
 **/
	 QPointer<QTextEdit> tbInfoTextEdit;

/**
 * Button: clears the screen
 **/
	 QPointer<QPushButton> clearScreenPushButton;

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
	QPointer<QGroupBox>** channelgrp;

/**
 * Layout which keeps one channel together
 **/
	QPointer<QHBoxLayout>** hbox;

	QPointer<QLabel> statusLabel;

	QPointer<QGroupBox> controlBox;

	QPointer<QVBoxLayout> controlLayout;
	QPointer<QVBoxLayout> scopeLayout;
	QPointer<QGroupBox> scopeGroup;

	QPointer<QScrollArea> allChScrollArea;
	QPointer<QVBoxLayout> allChScrollLayout;

	QPointer<QGridLayout> allChLayout;
	QPointer<QGroupBox> allChGroup;

	QPointer<QHBoxLayout> mainLayout;

	QPointer<QVBoxLayout> restLayout;

	QPointer<QGroupBox> restGroup;

	QPointer<QGroupBox> notchGroupBox;
	QPointer<QHBoxLayout> notchLayout;

	QPointer<QGroupBox> tbgrp;
	QPointer<QHBoxLayout> tbLayout;

	QPointer<QGroupBox> statusgrp;
	QPointer<QHBoxLayout> statusLayout;

	QPointer<QGroupBox> recGroupBox;
	QPointer<QHBoxLayout> recLayout;

	QPointer<QLabel> comment;
	QPointer<QLabel> recLabel;

	QFont* voltageFont;
	QFont* tbFont;
	QFont* commentFont;

private slots:
	void notchFreqChanged();
};


#endif
