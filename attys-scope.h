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
#include "samplingrate.h"
#include "AttysComm.h"
#include "AttysScan.h"
#include "audiobeep.h"
#include "pythonpipe.h"
#include "acceleration.h"

// version number
#define VERSION "2.6.2"

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
	 * Array for the special settings
	 **/
	QPointer<Special>** special;

	/**
	 * Array for the current setting
	 **/
	QPointer<Current>* current;

	/**
	 * Array for the acceleration
	 **/
	QPointer<Acceleration>* acceleration;

	/**
	 * Global sampling rate setting
	 **/
	QPointer<SamplingRate> samplingRate;
	
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
	 * switches on a tone when starting/stopping recording
	 **/
	QPointer<QCheckBox> toneCheckBox;


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

public:
	void slotSaveSettings();
	void slotLoadSettings();

	void slotRunPython();
	void slotStopPython();
	void slotLogPython() {logWindow->show();};

private:
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

public:
	/**
	 * Called when the UDP transmission is on/off
	 **/
	void slotStartUDP();
	void slotStopUDP();

private:
	int udpPort = 65000;

	void specialChanged();

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

	QPointer<QLabel> filenameLabel;

	QPointer<QVBoxLayout> controlLayout;
	QPointer<QVBoxLayout> scopeLayout;

	QPointer<QScrollArea> allChScrollArea;
	QPointer<QVBoxLayout> allChScrollLayout;

	QPointer<QGridLayout> allChLayout;
	QPointer<QGroupBox> allChGroup;

	QPointer<QHBoxLayout> mainLayout;

	QPointer<QVBoxLayout> restLayout;

	QPointer<QGroupBox> restGroup;

	QPointer<QHBoxLayout> tbLayout;

	QPointer<QHBoxLayout> statusLayout;

	QPointer<QHBoxLayout> recLayout;

	QPointer<QLabel> recLabel;

	QPointer<QLabel> tbLabel;

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

	AudioBeep* audiobeep;

	LogWindow* logWindow;
};


#endif
