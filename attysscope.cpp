#include <math.h>

#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QFileDialog>
#include <QSizePolicy>
#include <QSettings>
#include <QScrollArea>
#include <QMessageBox>
#include <qsplashscreen.h>

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <qtextedit.h>
#include <qfont.h>
#include <errno.h>

#include <iostream>
#include <fstream>

#include "scopewindow.h"
#include "attysscope.h"


Attys_scope::Attys_scope(QWidget *parent,
			 int ignoreSettings
	) : QWidget( parent ) {
	
	// to the get the stuff a bit closer together
	char styleSheet[] = "padding:0px;margin:0px;border:0px;";

	char styleSheetGroupBox[] = "padding:1px;margin:0px;border:0px";

	attysScopeWindow=new ScopeWindow(this);

	int channels = AttysComm::NCHANNELS;

	tb_us = 1000000 / attysScopeWindow->getActualSamplingRate();

	// fonts
	voltageFont = new QFont("Courier",10);
	QFontMetrics voltageMetrics(*voltageFont);

	tbFont = new QFont("Courier", 12);
	tbFont->setBold(true);
	QFontMetrics tbMetrics(*tbFont);

	// this the main layout which contains two sub-windows:
	// the control window and the oscilloscope window
	mainLayout = new QHBoxLayout;
	mainLayout->setSpacing(0);
	setLayout(mainLayout);

	// the corresponding box which contains all the controls
	controlBox = new QGroupBox ();
	controlBox->setStyleSheet(styleSheetGroupBox);
	controlBox->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Minimum ) );
	controlBox->setAttribute(Qt::WA_DeleteOnClose, false);

	// now we create another layout which contains all the controls
	controlLayout = new QVBoxLayout(controlBox);
	controlLayout->setSpacing(0);
	controlLayout->setMargin(0);

	// this is the vertical layout for all the controls
	scopeLayout = new QVBoxLayout(0);
	// the corresponding box which contains all the controls
	scopeGroup = new QGroupBox ();
	scopeGroup->setAttribute(Qt::WA_DeleteOnClose, false);

	// we create a scroll area
	allChScrollArea = new QScrollArea();
	allChScrollArea->setAttribute(Qt::WA_DeleteOnClose, false);
	// this is the layout containing the scrollArea
	allChScrollLayout = new QVBoxLayout();
	// to this layout we add just one widget which is the scroll area
	allChScrollLayout->addWidget(allChScrollArea);


	allChLayout = new QGridLayout;
	allChGroup = new QGroupBox;
	allChGroup->setStyleSheet(styleSheetGroupBox);
	allChGroup->setAttribute(Qt::WA_DeleteOnClose, false);
	allChLayout->setSpacing(0);
	allChGroup->setLayout(allChLayout);
	allChGroup->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Expanding ) );

	int row = 1;

	channelLabel=new QPointer<QLabel>*[nAttysDevices];
	channel=new QPointer<Channel>*[nAttysDevices];
	voltageTextEdit=new QPointer<QTextEdit>*[nAttysDevices];
	channelgrp=new QPointer<QGroupBox>*[nAttysDevices];
	hbox=new QPointer<QHBoxLayout>*[nAttysDevices];
	gain=new QPointer<Gain>*[nAttysDevices];
	highpass=new QPointer<Highpass>*[nAttysDevices];
	lowpass=new QPointer<Lowpass>*[nAttysDevices];
	bandstop = new QPointer<Bandstop>*[nAttysDevices];
	special = new QPointer<Special>*[nAttysDevices];
	current = new QPointer<Current>[nAttysDevices];
	specialLayout = new QPointer<QHBoxLayout>[nAttysDevices];
	
	AttysComm attysCommTmp(0);
	for(int n=0;n<nAttysDevices;n++) {
		channelLabel[n]=new QPointer<QLabel>[channels];
		channel[n]=new QPointer<Channel>[channels];
		voltageTextEdit[n]=new QPointer<QTextEdit>[channels];
		channelgrp[n]=new QPointer<QGroupBox>[channels];
		hbox[n]=new QPointer<QHBoxLayout>[channels];
		gain[n]=new QPointer<Gain>[channels];
		highpass[n]=new QPointer<Highpass>[channels];
		lowpass[n]=new QPointer<Lowpass>[channels];
		bandstop[n]=new QPointer<Bandstop>[channels];
		special[n] = new QPointer<Special>[2];
		current[n] = new Current();

		allChLayout->addWidget(new QLabel(attysName[n]), row, 1);
		row++;
		specialLayout[n] = new QHBoxLayout;
		for (int i = 0; i < 2; i++) {
			special[n][i] = new Special();
		}
		specialLayout[n]->addWidget(new QLabel("    "));
		specialLayout[n]->addWidget(new QLabel("CH1:"));
		specialLayout[n]->addWidget(special[n][0]);
		connect(special[n][0], SIGNAL(signalRestart()),
				this, SLOT(specialChanged()));
		specialLayout[n]->addWidget(new QLabel("    "));
		specialLayout[n]->addWidget(new QLabel("CH2:"));
		specialLayout[n]->addWidget(special[n][1]);
		connect(special[n][1], SIGNAL(signalRestart()),
			this, SLOT(specialChanged()));
		specialLayout[n]->addWidget(new QLabel(" "));
		specialLayout[n]->addWidget(current[n]);
		connect(current[n],SIGNAL(signalRestart()),
			this, SLOT(specialChanged()));
		specialLayout[n]->addWidget(new QLabel(" "));
		allChLayout->addLayout(specialLayout[n], row, 1);
		row++;

		for(int i=0;i<channels;i++) {
			// create the group for a channel
			char tmp[10];
			channelgrp[n][i] = new QGroupBox();
			channelgrp[n][i]->setStyleSheet(styleSheet);
			channelgrp[n][i]->setAttribute(Qt::WA_DeleteOnClose,false);
			// the corresponding layout
			hbox[n][i] = new QHBoxLayout();
			channelgrp[n][i]->setLayout(hbox[n][i]);
			sprintf(tmp,"%02d:",i);
			channelLabel[n][i] = new QLabel(tmp);
			channelLabel[n][i]->setStyleSheet(styleSheet);
			channelLabel[n][i]->setFont(*voltageFont);
			hbox[n][i]->addWidget(channelLabel[n][i]);
			hbox[n][i]->setSpacing(1);
			channel[n][i] = new Channel(channels,attysCommTmp.CHANNEL_SHORT_DESCRIPTION);
			channel[n][i] -> setChannel( i );
			channel[n][i]->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(channel[n][i]);
			voltageTextEdit[n][i]=new QTextEdit(channelgrp[n][i]);
			voltageTextEdit[n][i]->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(voltageTextEdit[n][i]);
			voltageTextEdit[n][i]->setFont(*voltageFont);
			char tmpVolt[128];
			sprintf(tmpVolt,"%f",1.0);
			voltageTextEdit[n][i]->setMaximumSize
				(voltageMetrics.width(tmpVolt),
				 (int)(voltageMetrics.height()*1.1));
			voltageTextEdit[n][i]->
				setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff ); 
			voltageTextEdit[n][i]->setReadOnly(true);
			voltageTextEdit[n][i]->setFont(*voltageFont);
			// voltageTextEdit[i]->setLineWidth(1);

			hbox[n][i]->addWidget(new QLabel("HP:"));
			highpass[n][i] = new Highpass(attysScopeWindow->getActualSamplingRate(),-1);
			highpass[n][i] ->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(highpass[n][i]);

			hbox[n][i]->addWidget(new QLabel("LP:"));
			lowpass[n][i] = new Lowpass(attysScopeWindow->getActualSamplingRate(),0);
			lowpass[n][i] ->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(lowpass[n][i]);

			hbox[n][i]->addWidget(new QLabel("BS:"));
			bandstop[n][i] = new Bandstop(attysScopeWindow->getActualSamplingRate(), 0);
			bandstop[n][i]->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(bandstop[n][i]);

			gain[n][i] = new Gain();
			gain[n][i]->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(gain[n][i]);

			allChLayout->addWidget(channelgrp[n][i],row,1);
			row++;
		}
	}

	controlLayout->addWidget(allChScrollArea);
	allChScrollArea->setWidget(allChGroup);

	// now we create another layout which contains all the remaining controls
	restLayout = new QVBoxLayout;
	// the corresponding box which contains all the controls
	restGroup = new QGroupBox;
	restGroup->setStyleSheet(styleSheetGroupBox);
	restGroup->setAttribute(Qt::WA_DeleteOnClose, false);

	// group for the record stuff
	recGroupBox = new QGroupBox();
	recGroupBox->setStyleSheet(styleSheetGroupBox);
	recGroupBox->setAttribute(Qt::WA_DeleteOnClose, false);
	recLayout = new QHBoxLayout();

	recLabel = new QLabel("Write to file:");
	recLayout->addWidget(recLabel);

	filePushButton = new QPushButton( "&filename" );
	filePushButton->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						    QSizePolicy::Fixed ));
	filePushButton->setStyleSheet(
		"background-color: white;border-style:outset;border-width: 1px;border-color: grey;font: bold 12px; padding: 3px;");
	connect(filePushButton, SIGNAL( clicked() ),
		this, SLOT( enterFileName() ) );
	recLayout->addWidget(filePushButton);
	recLayout->addWidget(new QLabel("    "));

	recCheckBox = new QCheckBox( "REC" );
	recCheckBox->connect(recCheckBox, SIGNAL( stateChanged(int) ),
			       this, SLOT( recstartstop(int) ) );
	recCheckBox->setEnabled( false );
	recLayout->addWidget(recCheckBox);
	recCheckBox->setStyleSheet("font: bold 18px; padding: 3px;");

	recGroupBox->setLayout(recLayout);
	restLayout->addWidget(recGroupBox);


	// group for the UDP stuff
	udpGroupBox = new QGroupBox();
	udpGroupBox->setStyleSheet(styleSheetGroupBox);
	udpGroupBox->setAttribute(Qt::WA_DeleteOnClose, false);
	udpLayout = new QHBoxLayout();

	udpLayout->addWidget(new QLabel("UDP broadcast on port "));

	udpTextEdit = new QTextEdit("65000");
	udpTextEdit->setFont(*tbFont);
	udpTextEdit->setMaximumHeight(tbMetrics.height() * 1.5);
	udpTextEdit->setMaximumWidth(tbMetrics.width('X') * 14);
	udpTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	udpLayout->addWidget(udpTextEdit);

	udpCheckBox = new QCheckBox("&Broadcast");
	udpCheckBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
		QSizePolicy::Fixed));
	connect(udpCheckBox, SIGNAL(clicked()),
		this, SLOT(udpTransmit()));
	udpLayout->addWidget(udpCheckBox);

	udpGroupBox->setLayout(udpLayout);
	restLayout->addWidget(udpGroupBox);

	// group for the time base
	tbgrp = new QGroupBox();
	udpGroupBox->setStyleSheet(styleSheetGroupBox);
	tbgrp->setAttribute(Qt::WA_DeleteOnClose, false);
	tbLayout = new QHBoxLayout;

	QLabel tbLabel("Timebase:");
	tbLayout->addWidget(&tbLabel);

	tbIncPushButton = new QPushButton( "slower" );

	char tbStyle[]="background-color: white;border-style:outset;border-width: 1px;border-color: black;font: bold 12px;padding: 3px;";
	tbIncPushButton->setStyleSheet(tbStyle);
	tbIncPushButton->setFont(*tbFont);
	tbgrp->connect(tbIncPushButton, SIGNAL( clicked() ),
		this, SLOT( incTbEvent() ) );
	tbLayout->addWidget(tbIncPushButton);

	tbDecPushButton = new QPushButton( "faster" );
	tbDecPushButton->setStyleSheet(tbStyle);
	tbDecPushButton->setFont(*tbFont);	
	tbgrp->connect(tbDecPushButton, SIGNAL( clicked() ),
		       this, SLOT( decTbEvent() ) );
	tbLayout->addWidget(tbDecPushButton);

	tbInfoTextEdit = new QTextEdit(tbgrp);
	tbInfoTextEdit->setFont (*tbFont);
	QFontMetrics metricsTb(*tbFont);
	tbInfoTextEdit->setMaximumHeight ( tbMetrics.height() * 1.5 );
	tbInfoTextEdit->setMaximumWidth ( tbMetrics.width('X') * 14 );
	tbInfoTextEdit->setReadOnly(true);
	tbInfoTextEdit->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	tbLayout->addWidget(tbInfoTextEdit);

	tbResetPushButton = new QPushButton( "clear" );
	tbResetPushButton->setStyleSheet(tbStyle);
	tbResetPushButton->setFont(*tbFont);	
	tbgrp->connect(tbResetPushButton, SIGNAL( clicked() ),
		       this, SLOT( resetTbEvent() ) );
	tbLayout->addWidget(tbResetPushButton);

	displayCheckbox = new QCheckBox();
	displayCheckbox->setChecked(1);
	tbLayout->addWidget(new QLabel("Running:"));
	tbLayout->addWidget(displayCheckbox);

	tbgrp->setLayout(tbLayout);
	restLayout->addWidget(tbgrp);

	statusgrp = new QGroupBox;
	statusgrp->setStyleSheet(styleSheetGroupBox);
	statusgrp->setAttribute(Qt::WA_DeleteOnClose, false);
	statusLayout = new QHBoxLayout;

	sprintf(status,"# of Attys devs: %d, Sampling rate: %d Hz.",
		nAttysDevices,
		attysComm[0]->getSamplingRateInHz());
	statusLabel = new QLabel(status);
	statusLayout->addWidget(statusLabel);
	statusgrp->setLayout(statusLayout);
	restLayout->addWidget(statusgrp);

	restGroup->setLayout(restLayout);
	controlLayout->addWidget(restGroup);
	restGroup->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Fixed ) );

	controlBox->setLayout(controlLayout);

	attysScopeWindow->setMinimumWidth ( 400 );
	attysScopeWindow->setMinimumHeight ( 200 );

	scopeLayout->addWidget(attysScopeWindow);
	scopeGroup->setLayout(scopeLayout);

	allChScrollArea->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						     QSizePolicy::Expanding ) );
	allChScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	allChScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	mainLayout->addWidget(controlBox);
	mainLayout->addWidget(scopeGroup);

	changeTB();

	if (!ignoreSettings) {
		readSettings();
	}

	attysScopeWindow->startDAQ();
}


// config constants
#define SETTINGS_GLOBAL "global"
#define SETTINGS_CHANNELS "channelconfig"
#define SETTINGS_SPECIAL "special_config%09d_ch%09d"
#define SETTINGS_CURRENT "current_config%09d"
#define CHSETTING_FORMAT "ch_mapping_dev%09d_ch%09d"
#define HIGHPASS_SETTING_FORMAT "highpass_dev%09d_ch%09d"
#define LOWPASS_SETTING_FORMAT "lowpass_dev%09d_ch%09d"
#define BANDSTOP_SETTING_FORMAT "bandstop_dev%09d_ch%09d"
#define GAIN_SETTING_FORMAT "gain_mapping_dev%09d_ch%09d"

void Attys_scope::readSettings() {
	QSettings settings(QSettings::IniFormat,
		QSettings::UserScope,
		ATTYS_STRING,
		PROGRAM_NAME);

	int channels = AttysComm::NCHANNELS;
	int nch_enabled = 0;

	settings.beginGroup(SETTINGS_CHANNELS);

	for (int n = 0; n < nAttysDevices; n++) {
		for (int i = 0; i < 2; i++) {
			char tmpSp[256];
			sprintf(tmpSp, SETTINGS_SPECIAL, n, i);
			int a = settings.value(tmpSp, 0).toInt();
			_RPT1(0, "settings special %d\n", a);
			special[n][i]->setSpecial(a);
		}
		current[n] = new Current();
		char tmpCur[256];
		sprintf(tmpCur, SETTINGS_CURRENT, n);
		int a = settings.value(tmpCur, 0).toInt();
		_RPT1(0, "settings current %d\n", a);
		current[n]->setCurrent(a);

		char tmpCh[128];
		for (int i = 0; i < channels; i++) {

			sprintf(tmpCh, CHSETTING_FORMAT, n, i);
			channel[n][i]->setChannel(
				settings.value(tmpCh, i).toInt());
			if (channel[n][i]->isActive())
				nch_enabled++;

			sprintf(tmpCh, HIGHPASS_SETTING_FORMAT, n, i);
			float fhp = settings.value(tmpCh, -1.0).toFloat();
			//_RPT2(0, "hp %d= %f\n", i,fhp);
			highpass[n][i]->setFrequency(fhp);

			sprintf(tmpCh, LOWPASS_SETTING_FORMAT, n, i);
			float flp = settings.value(tmpCh, 0.0).toFloat();
			//_RPT2(0, "lp %d= %f\n", i,flp);
			lowpass[n][i]->setFrequency(flp);

			sprintf(tmpCh, BANDSTOP_SETTING_FORMAT, n, i);
			float fbs = settings.value(tmpCh, 0.0).toFloat();
			//_RPT2(0, "lp %d= %f\n", i,flp);
			bandstop[n][i]->setFrequency(fbs);

			sprintf(tmpCh, GAIN_SETTING_FORMAT, n, i);
			gain[n][i]->setGain(
				settings.value(tmpCh, 1.0).toFloat());
		}
	}
	settings.endGroup();
	// at least one should be active not to make the user nervous.
	if (nch_enabled == 0)
		channel[0][0]->setChannel(0);
}

void Attys_scope::setInfo(const char * txt)
{
	QString t = txt;
	QString s = status;
	statusLabel->setText(status+t);
}


Attys_scope::~Attys_scope() {
	QSettings settings(QSettings::IniFormat, 
			   QSettings::UserScope,
			   ATTYS_STRING,
			   PROGRAM_NAME);

	int channels = AttysComm::NCHANNELS;
	settings.beginGroup(SETTINGS_CHANNELS);
	for(int n=0;n<nAttysDevices;n++) {
		for(int i=0;i<channels;i++) {
			char tmp[128];

			sprintf(tmp,CHSETTING_FORMAT,n,i);
			settings.setValue(tmp, 
					  channel[n][i] -> getChannel() );

			sprintf(tmp, GAIN_SETTING_FORMAT, n, i);
			settings.setValue(tmp,
				gain[n][i]->getGain());

			sprintf(tmp, HIGHPASS_SETTING_FORMAT, n, i);
			settings.setValue(tmp,
				highpass[n][i]->getFrequency());

			sprintf(tmp, LOWPASS_SETTING_FORMAT, n, i);
			float flp = lowpass[n][i]->getFrequency();
			settings.setValue(tmp,flp);

			sprintf(tmp, BANDSTOP_SETTING_FORMAT, n, i);
			float fbs = bandstop[n][i]->getFrequency();
			settings.setValue(tmp, fbs);
		}
		for (int i = 0; i < 2; i++) {
			char tmpSp[256];
			sprintf(tmpSp, SETTINGS_SPECIAL, n, i);
			settings.setValue(tmpSp, special[n][i]->getSpecial());
		}
		char tmpCur[256];
		sprintf(tmpCur, SETTINGS_CURRENT, n);
		settings.setValue(tmpCur, current[n]->getCurrent());
	}
	settings.endGroup();
}


void Attys_scope::disableControls() {
	filePushButton->setEnabled( false );
	int channels = AttysComm::NCHANNELS;
	for(int n=0;n<nAttysDevices;n++) {
		for(int i=0;i<channels;i++) {
			channel[n][i]->setEnabled( false );
		}
	}
}


void Attys_scope::enableControls() {
	filePushButton->setEnabled( true );
	int channels = AttysComm::NCHANNELS;
	for(int n=0;n<nAttysDevices;n++) {
		for(int i=0;i<channels;i++) {
			channel[n][i]->setEnabled( true );
		}
	}
}


void Attys_scope::setFilename(QString fn,int tsv) {
	attysScopeWindow->setFilename(fn,tsv);
	QString tmp;
	tmp="attys_scope - datafile: "+fn;
	setWindowTitle( tmp );
	recCheckBox->setEnabled( true );
}


void Attys_scope::enterFileName() {
	QFileDialog::Options options;
	QString filters(tr("tab separated values (*.tsv);;"
			   "comma separated values (*.csv)"
				));

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(filters);
	dialog.setViewMode(QFileDialog::Detail);

	if (dialog.exec()) {
		QString fileName = dialog.selectedFiles()[0];
		QString extension = dialog.selectedNameFilter();
		extension = extension.mid(extension.indexOf("."), 4);
		if (fileName.indexOf(extension) == -1) {
			fileName = fileName + extension;
		}
		int isTSV = extension.indexOf("tsv");
		setFilename(QString(fileName), isTSV > -1);
	}
}

// callback
void Attys_scope::recstartstop(int) 
{
  if (recCheckBox->checkState()==Qt::Checked) 
    {
      attysScopeWindow->startRec();
    } 
  else 
    {
      attysScopeWindow->stopRec();
      // to force the user to enter a new filename
      recCheckBox->setEnabled( false );
    }
}

void Attys_scope::incTbEvent() {
	if (tb_us<1000000) {
		tb_us=tb_us*2;
		changeTB();
	}
}


void Attys_scope::decTbEvent() {
	int minTBvalue = 1000000 / attysScopeWindow->getActualSamplingRate();
	if (minTBvalue < 1) minTBvalue = 1;
	if (tb_us > minTBvalue) {
		tb_us=tb_us/2;
		changeTB();
	}
}


void Attys_scope::changeTB() {
	QString s;
	if (tb_us<1000) {
		s.sprintf( "%d usec", tb_us);
	} else if (tb_us<1000000) {
		s.sprintf( "%d msec", tb_us/1000);
	} else {
		s.sprintf( "%d sec", tb_us/1000000);
	}		
	tbInfoTextEdit->setText(s);
	attysScopeWindow->setTB(tb_us);
}

void Attys_scope::resetTbEvent() {
	    attysScopeWindow->clearScreen();
};


void Attys_scope::udpTransmit() {
	if (udpCheckBox->isChecked()) {
		udpTextEdit->setEnabled(0);
		int port = udpTextEdit->toPlainText().toInt();
		attysScopeWindow->startUDP(port);
	}
	else {
		attysScopeWindow->stopUDP();
		udpTextEdit->setEnabled(1);
	}
};


void Attys_scope::specialChanged() {
	if (restartInfo) {
		QMessageBox msgBox;
		msgBox.setText("You need to restart Attys Scope that these changes take effect.");
		msgBox.exec();
		restartInfo = 0;
	}
};



int main( int argc, char **argv )
{
	// default values
	int ignoreSettings = 0;

	QSettings settings(QSettings::IniFormat, 
			   QSettings::UserScope,
			   ATTYS_STRING,
			   PROGRAM_NAME);

	QApplication a( argc, argv );		// create application object

	QPixmap pixmap(":/attys.png");
	QSplashScreen* splash = new QSplashScreen(pixmap);
	splash->setFont( QFont("Helvetica", 12, QFont::Bold) );
	splash->show();
	splash->showMessage("Scanning for paired devices");
	a.processEvents();

	// see if we have any Attys!
	int ret = attysScan(splash);

	// zero on success and non zero on failure
	if (ret) {
		a.processEvents();
		Sleep(1000);
		delete splash;
		return ret;
	}
	
	// none detected
	if (nAttysDevices<1) {
		printf("No Attys present or not paired.\n");
		splash->showMessage("Cound not connect\nand/or no devices paired.");
		a.processEvents();
		Sleep(5000);
		delete splash;
		return -1;
	}

	for(int i = 0;i<argc;i++) {
		if (strstr(argv[i],"/i")) ignoreSettings = 1;	
		if (strstr(argv[i],"-i")) ignoreSettings = 1;
	}

	Attys_scope attys_scope(0,ignoreSettings);

	// show widget
	attys_scope.show();
	splash->finish(&attys_scope);
	delete splash;
	// run event loop
	return a.exec();
}
