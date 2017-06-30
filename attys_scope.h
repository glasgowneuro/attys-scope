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
#include "special.h"
#include "current.h"
#include "highpass.h"
#include "lowpass.h"
#include "bandstop.h"

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
		      int num_of_devices = 1
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

	QPointer<QCheckBox> displayCheckbox;

	/**
	* Array for the special settings
	**/
	QPointer<Special>** special;

	/**
	* Array for the current setting
	**/
	QPointer<Current>* current;


	/**
 * channel label
 **/
	QPointer<QLabel>** channelLabel;

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
     * Array for the channel settings
     **/
	QPointer<Channel>** channel;

    /**
     * Array for the gain settings
     **/
	QPointer<Gain>** gain;

    /**
     * highpass filter
     **/
	QPointer<Highpass>** highpass;

    /**
     * lowpass filter
     **/
	QPointer<Lowpass>** lowpass;

	/**
	* lowpass filter
	**/
	QPointer<Bandstop>** bandstop;

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
	QPointer<QHBoxLayout>* specialLayout;

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

	private:
		void readSettings();
};


#endif
