class Attys_scope;
#ifndef ATTYS_SCOPE_H
#define ATTYS_SCOPE_H

#include <QWidget>
#include <QPainter>
#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout> 
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QSettings>
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
#include "recbutton.h"
#include "AttysComm.h"
#include "AttysScan.h"

// version number
#define VERSION "2.0.4"

#define ATTYS_STRING "ATTYS"
#define PROGRAM_NAME "attys-scope"
#define EXECUTABLE_NAME "attys-scope"


struct AttysScanMsg : public AttysScanListener {
	QSplashScreen* splash = NULL;
	QApplication* app = NULL;
        virtual void message(const int, const char * msg) {
		fprintf(stderr,"%s\n",msg);
		if (splash) {
			splash->showMessage(msg);
			if (app) {
				app->processEvents();
			}
		}
	}
};


class Attys_scope : public QWidget
{
	
	Q_OBJECT
public:
/**
 * Constructor
 **/
	Attys_scope( QWidget *parent,
		     int ignoreSettings = 0
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

	/**
	* Button which controls recording
	**/
    QPointer<RecButton> recCheckBox;

	/**
	 * Button which controls recording
	 **/
	QPointer<QCheckBox> vers1dataCheckBox;

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

public:
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
	QPointer<QLineEdit> timeInfoLineEdit;

    /**
     * Text-field for the file-name
     **/
	QPointer<QPushButton> filePushButton;

	/**
	* port for UDP
	**/
	QPointer<QLineEdit> udpLineEdit;

	/**
	* switches on UDP broadcast
	**/
	QPointer<QCheckBox> udpCheckBox;


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
 * Text-field: time between samples
 **/
	 QPointer<QLineEdit> tbInfoLineEdit;

/**
 * Button: clears the screen
 **/
	 QPointer<QPushButton> clearScreenPushButton;

	 QPointer<QPushButton> savePushButton;
	 QPointer<QPushButton> loadPushButton;

private slots:
    void slotSaveSettings();
    void slotLoadSettings();

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

	/**
	* Called when the UDP transmission is on/off
	**/
	void udpTransmit();

	void specialChanged();

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
 * Layout which keeps one channel together
 **/
	QPointer<QHBoxLayout>** hbox;
	QPointer<QHBoxLayout>* specialLayout;

	QPointer<QLabel> statusLabel;

	QPointer<QVBoxLayout> controlLayout;
	QPointer<QVBoxLayout> scopeLayout;

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

	QPointer<QGroupBox> udpGroupBox;
	QPointer<QHBoxLayout> udpLayout;

	QPointer<QLabel> recLabel;

	QFont* voltageFont;
	QFont* tbFont;

	private:
		void readSettings(QSettings &settings);
		void writeSettings(QSettings &settings);

	public:
		void setInfo(const char* txt);

	private:
		int restartInfo = 1;

		char status[256];


};


#endif
