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
#include <QInputDialog>

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <qtextedit.h>
#include <qfont.h>
#include <errno.h>

#include <iostream>
#include <fstream>

#include "scopewindow.h"
#include "attys-scope.h"
#include "stylesheets.h"

Attys_scope::Attys_scope(QWidget *parent,
			 int ignoreSettings
	) : QWidget( parent ) {

	attysScopeWindow=new ScopeWindow(this);

	audiobeep = new AudioBeep(this,1,1000,0.5);

	int channels = AttysComm::NCHANNELS;

	// this the main layout which contains two sub-windows:
	// the control window and the oscilloscope window
	mainLayout = new QHBoxLayout;
	setLayout(mainLayout);

	// now we create another layout which contains all the controls
	controlLayout = new QVBoxLayout;

	// this is the vertical layout for all the controls
	scopeLayout = new QVBoxLayout(0);

	allChLayout = new QGridLayout;
	allChGroup = new QGroupBox;
	allChGroup->setAttribute(Qt::WA_DeleteOnClose, false);
	allChGroup->setLayout(allChLayout);
	allChGroup->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Expanding ) );

	int row = 1;

	channelLabel=new QPointer<QLabel>*[attysScan.getNAttysDevices()];
	channel=new QPointer<Channel>*[attysScan.getNAttysDevices()];
	hbox=new QPointer<QHBoxLayout>*[attysScan.getNAttysDevices()];
	gain=new QPointer<Gain>*[attysScan.getNAttysDevices()];
	highpass=new QPointer<Highpass>*[attysScan.getNAttysDevices()];
	lowpass=new QPointer<Lowpass>*[attysScan.getNAttysDevices()];
	bandstop = new QPointer<Bandstop>*[attysScan.getNAttysDevices()];
	special = new QPointer<Special>*[attysScan.getNAttysDevices()];
	current = new QPointer<Current>[attysScan.getNAttysDevices()];
	specialLayout = new QPointer<QHBoxLayout>[attysScan.getNAttysDevices()];

	bool isHighSpeed = 0;

	AttysComm attysCommTmp;
	for(int n=0;n<attysScan.getNAttysDevices();n++) {
		channelLabel[n]=new QPointer<QLabel>[channels];
		channel[n]=new QPointer<Channel>[channels];
		hbox[n]=new QPointer<QHBoxLayout>[channels];
		gain[n]=new QPointer<Gain>[channels];
		highpass[n]=new QPointer<Highpass>[channels];
		lowpass[n]=new QPointer<Lowpass>[channels];
		bandstop[n]=new QPointer<Bandstop>[channels];
		special[n] = new QPointer<Special>[2];
		current[n] = new Current();
		current[n]->setStyleSheet(styleSheetChannel);
		if (strstr(attysScan.getAttysName(n),"ATTYS2") != NULL ) {
			isHighSpeed = isHighSpeed || true;
		}
		allChLayout->addWidget(new QLabel(QString::asprintf("Attys #%d (%s)\n",n,attysScan.getAttysName(n))), row, 1);
		row++;
		specialLayout[n] = new QHBoxLayout;
		for (int i = 0; i < 2; i++) {
			special[n][i] = new Special();
		}
		specialLayout[n]->addWidget(new QLabel("    "));
		specialLayout[n]->addWidget(new QLabel("CH1:"));
		specialLayout[n]->addWidget(special[n][0]);
		connect(special[n][0],&Special::signalRestart,
			this, &Attys_scope::specialChanged);
		specialLayout[n]->addWidget(new QLabel("    "));
		specialLayout[n]->addWidget(new QLabel("CH2:"));
		specialLayout[n]->addWidget(special[n][1]);
		connect(special[n][1],&Special::signalRestart,
			this, &Attys_scope::specialChanged);
		specialLayout[n]->addWidget(new QLabel(" "));
		specialLayout[n]->addWidget(current[n]);
		connect(current[n],&Current::signalRestart,
			this, &Attys_scope::specialChanged);
		specialLayout[n]->addWidget(new QLabel(" "));
		allChLayout->addLayout(specialLayout[n], row, 1);
		row++;

		for(int i=0;i<channels;i++) {
			// create the group for a channel
			char tmp[10];
			// the corresponding layout
			hbox[n][i] = new QHBoxLayout();
			hbox[n][i]->setMargin(5);
			
			sprintf(tmp,"%02d:",i);
			channelLabel[n][i] = new QLabel(tmp);
			channelLabel[n][i]->setStyleSheet(styleSheetNoPadding);
			hbox[n][i]->addWidget(channelLabel[n][i]);

			channel[n][i] = new Channel(channels,attysCommTmp.CHANNEL_SHORT_DESCRIPTION);
			channel[n][i] -> setChannel( i );
			channel[n][i]->setStyleSheet(styleSheetChannel);
			int w = channel[n][i]->minimumSizeHint().width();
			channel[n][i]->setMinimumWidth(w * 1.1);
			hbox[n][i]->addWidget(channel[n][i]);

			QPointer<QLabel> hp = new QLabel("HP:");
			hp->setStyleSheet(styleSheetLabel);
			hbox[n][i]->addWidget(hp);
			highpass[n][i] = new Highpass();
			highpass[n][i] ->setStyleSheet(styleSheetCombo);
			hbox[n][i]->addWidget(highpass[n][i]);

			QPointer<QLabel> lp = new QLabel("LP:");
			lp->setStyleSheet(styleSheetLabel);
			hbox[n][i]->addWidget(lp);
			lowpass[n][i] = new Lowpass();
			lowpass[n][i] ->setStyleSheet(styleSheetCombo);
			hbox[n][i]->addWidget(lowpass[n][i]);

			QPointer<QLabel> bs = new QLabel("BS:");
			bs->setStyleSheet(styleSheetLabel);
			hbox[n][i]->addWidget(bs);
			bandstop[n][i] = new Bandstop();
			bandstop[n][i]->setStyleSheet(styleSheetCombo);
			hbox[n][i]->addWidget(bandstop[n][i]);

			hbox[n][i]->addWidget(new QLabel(" "));
			gain[n][i] = new Gain();
			gain[n][i]->setStyleSheet(styleSheetGain);
			w = gain[n][i]->minimumSizeHint().width();
			gain[n][i]->setMinimumWidth(w * 1.1);
			hbox[n][i]->addWidget(gain[n][i]);

			allChLayout->addLayout(hbox[n][i],row,1);
			row++;
		}
	}

	// we create a scroll area
	allChScrollArea = new QScrollArea();
	allChScrollArea->setWidgetResizable(false);
	allChScrollArea->setAttribute(Qt::WA_DeleteOnClose, false);
	allChScrollArea->setWidget(allChGroup);
	// this is the layout containing the scrollArea
	allChScrollLayout = new QVBoxLayout();
	// to this layout we add just one widget which is the scroll area
	allChScrollLayout->addWidget(allChScrollArea);

	controlLayout->addWidget(allChScrollArea);

	// now we create another layout which contains all the remaining controls
	restLayout = new QVBoxLayout;
	restLayout->setSpacing(10);
	restLayout->setMargin(10);
	restLayout->setSizeConstraint(QLayout::SetFixedSize);

	restGroup = new QGroupBox;
	restGroup->setAttribute(Qt::WA_DeleteOnClose, false);
	restGroup->setLayout(restLayout);
	restGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
		QSizePolicy::Expanding));

	recLayout = new QHBoxLayout();

	recLabel = new QLabel("Write to file:");
	recLayout->addWidget(recLabel);

	filePushButton = new QPushButton( "filename" );
	filePushButton->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						    QSizePolicy::Fixed ));
	connect(filePushButton,&QPushButton::clicked,
		this,&Attys_scope::enterFileName);
	recLayout->addWidget(filePushButton);

	recLayout->addWidget(new QLabel("    "));
	recCheckBox = new RecButton();
	recCheckBox->connect(recCheckBox,&RecButton::stateChanged,
			     this,&Attys_scope::recstartstop);
	recCheckBox->setEnabled( false );
	recLayout->addWidget(recCheckBox);
	recCheckBox->setStyleSheet(styleCheckBox);

	toneCheckBox = new QCheckBox("1kHz beep");
	toneCheckBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Fixed));
	toneCheckBox->setVisible(audiobeep->isOK());
	recLayout->addWidget(toneCheckBox);

	restLayout->addLayout(recLayout);

	tbLayout = new QHBoxLayout();

	tbLabel = new QLabel("Timebase: ");
	tbLayout->addWidget(tbLabel);

	tbIncPushButton = new QPushButton( "slower" );

	connect(tbIncPushButton,&QPushButton::clicked,
		       this,&Attys_scope::incTbEvent);
	
	tbLayout->addWidget(tbIncPushButton);

	tbDecPushButton = new QPushButton( "faster" );
	connect(tbDecPushButton,&QPushButton::clicked,
		this,&Attys_scope::decTbEvent);
	tbLayout->addWidget(tbDecPushButton);

	tbInfoLineEdit = new QLineEdit();
	tbInfoLineEdit->setReadOnly(true);
	tbInfoLineEdit->setStyleSheet(styleLineEdit);
	tbLayout->addWidget(tbInfoLineEdit);

	samplingRate = new SamplingRate(isHighSpeed);
	connect(samplingRate,&SamplingRate::signalRestart,
		this,&Attys_scope::specialChanged);
	samplingRate->setStyleSheet(styleSheetChannel);
	tbLayout->addWidget(samplingRate);

	restLayout->addLayout(tbLayout);

	statusLayout = new QHBoxLayout();

	restLayout->addLayout(statusLayout);

	controlLayout->addWidget(restGroup);

	attysScopeWindow->setMinimumWidth ( 500 );
	attysScopeWindow->setMinimumHeight ( 200 );

	scopeLayout->addWidget(attysScopeWindow);

	allChScrollArea->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Expanding ) );
	allChScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	statusLabel = new QLabel();
	controlLayout->addWidget(statusLabel);

	attysScopeWindow->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
		QSizePolicy::Expanding));
	mainLayout->addLayout(controlLayout);
	mainLayout->addWidget(attysScopeWindow);

	if (!ignoreSettings) {
		QSettings settings(QSettings::IniFormat,
			QSettings::UserScope,
			ATTYS_STRING,
			PROGRAM_NAME);
		readSettings(settings);
	}

	attysScopeWindow->startDAQ();

	tb_us = 1000000 / attysScopeWindow->getActualSamplingRate();
	changeTB();

	if (attysScopeWindow->getActualSamplingRate() > 250) {
		sprintf(status, "%d Attys -- Magnetometer is off at fs>250Hz", attysScan.getNAttysDevices());		
	} else {
		sprintf(status, "%d Attys", attysScan.getNAttysDevices());
	}
	statusLabel->setText(status);
}


// config constants
#define SETTINGS_UDP "udp"
#define SETTINGS_UDP_PORT "port"
#define SETTINGS_UDP_ON "transmit"
#define SETTINGS_CHANNELS "channelconfig"
#define SETTINGS_SPECIAL "special_config%09d_ch%09d"
#define SETTINGS_CURRENT "current_config%09d"
#define CHSETTING_FORMAT "ch_mapping_dev%09d_ch%09d"
#define HIGHPASS_SETTING_FORMAT "highpass_dev%09d_ch%09d"
#define LOWPASS_SETTING_FORMAT "lowpass_dev%09d_ch%09d"
#define BANDSTOP_SETTING_FORMAT "bandstop_dev%09d_ch%09d"
#define GAIN_SETTING_FORMAT "gain_mapping_dev%09d_ch%09d"
#define SETTINGS_SAMPLING_RATE "fs"

void Attys_scope::readSettings(QSettings &settings) {
	int channels = AttysComm::NCHANNELS;
	int nch_enabled = 0;

	settings.beginGroup(SETTINGS_UDP);
	udpPort = settings.value(SETTINGS_UDP_PORT, 65000).toInt();
	settings.endGroup();

	settings.beginGroup(SETTINGS_CHANNELS);

	samplingRate->setSamplingRate(settings.value(SETTINGS_SAMPLING_RATE,AttysCommBase::ADC_RATE_250HZ).toInt());

	for (int n = 0; n < attysScan.getNAttysDevices(); n++) {
		for (int i = 0; i < 2; i++) {
			char tmpSp[256];
			sprintf(tmpSp, SETTINGS_SPECIAL, n, i);
			int a = settings.value(tmpSp, 0).toInt();
			// _RPT1(0, "settings special %d\n", a);
			special[n][i]->setSpecial(a);
		}
		char tmpCur[256];
		sprintf(tmpCur, SETTINGS_CURRENT, n);
		int a = settings.value(tmpCur, 0).toInt();
		// _RPT1(0, "settings current %d\n", a);
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
			highpass[n][i]->setSamplingRate(samplingRate->getSamplingRateInHz());
			highpass[n][i]->setFrequency(fhp);

			sprintf(tmpCh, LOWPASS_SETTING_FORMAT, n, i);
			float flp = settings.value(tmpCh, 0.0).toFloat();
			//_RPT2(0, "lp %d= %f\n", i,flp);
			lowpass[n][i]->setSamplingRate(samplingRate->getSamplingRateInHz());
			lowpass[n][i]->setFrequency(flp);

			sprintf(tmpCh, BANDSTOP_SETTING_FORMAT, n, i);
			float fbs = settings.value(tmpCh, 0.0).toFloat();
			//_RPT2(0, "lp %d= %f\n", i,flp);
			bandstop[n][i]->setSamplingRate(samplingRate->getSamplingRateInHz());
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

void Attys_scope::writeSettings(QSettings & settings)
{

	settings.beginGroup(SETTINGS_UDP);
	settings.setValue(SETTINGS_UDP_PORT, udpPort);
	settings.endGroup();

	int channels = AttysComm::NCHANNELS;
	settings.beginGroup(SETTINGS_CHANNELS);

	settings.setValue(SETTINGS_SAMPLING_RATE,samplingRate->getSamplingRate());
	
	for (int n = 0; n<attysScan.getNAttysDevices(); n++) {
		for (int i = 0; i<channels; i++) {
			char tmp[128];

			sprintf(tmp, CHSETTING_FORMAT, n, i);
			settings.setValue(tmp,
				channel[n][i]->getChannel());

			sprintf(tmp, GAIN_SETTING_FORMAT, n, i);
			settings.setValue(tmp,
				gain[n][i]->getGain());

			sprintf(tmp, HIGHPASS_SETTING_FORMAT, n, i);
			settings.setValue(tmp,
				highpass[n][i]->getFrequency());

			sprintf(tmp, LOWPASS_SETTING_FORMAT, n, i);
			float flp = lowpass[n][i]->getFrequency();
			settings.setValue(tmp, flp);

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
		//		fprintf(stderr,"curr=%d\n",current[n]->getCurrent());
		settings.setValue(tmpCur, current[n]->getCurrent());
	}
	settings.endGroup();
}


void Attys_scope::slotSaveSettings() {
	QString filters(tr("configuration files (*.ini)"));

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(filters);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);

	if (dialog.exec()) {
		QString fileName = dialog.selectedFiles()[0];
		QString extension = dialog.selectedNameFilter();
		extension = extension.mid(extension.indexOf("."), 4);
		if (fileName.indexOf(extension) == -1) {
			fileName = fileName + extension;
		}
		QSettings settings(fileName, QSettings::IniFormat);
		writeSettings(settings);
	}
};

void Attys_scope::slotLoadSettings() {
	QString filters(tr("configuration files (*.ini)"));

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(filters);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);

	if (dialog.exec()) {
		QString fileName = dialog.selectedFiles()[0];
		QSettings settings(fileName, QSettings::IniFormat);
		readSettings(settings);
	}
};

void Attys_scope::slotRunPython() {
	QString filters(tr("Python scripts (*.py)"));

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(filters);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);

	if (dialog.exec()) {
		QString fileName = dialog.selectedFiles()[0];
		attysScopeWindow->startPython(fileName);
	}
}

void Attys_scope::slotStopPython() {
	attysScopeWindow->stopPython();
}

void Attys_scope::setInfo(const char * txt)
{
	QString t = txt;
	statusLabel->setText(status+QString(" ")+t);
}


Attys_scope::~Attys_scope() {
	QSettings settings(QSettings::IniFormat, 
			   QSettings::UserScope,
			   ATTYS_STRING,
			   PROGRAM_NAME);

	writeSettings(settings);
	delete audiobeep;
}


void Attys_scope::disableControls() {
	filePushButton->setEnabled( false );
	int channels = AttysComm::NCHANNELS;
	for(int n=0;n<attysScan.getNAttysDevices();n++) {
		for(int i=0;i<channels;i++) {
			channel[n][i]->setEnabled( false );
		}
	}
}


void Attys_scope::enableControls() {
	filePushButton->setEnabled( true );
	int channels = AttysComm::NCHANNELS;
	for(int n=0;n<attysScan.getNAttysDevices();n++) {
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
	QString filters(tr("tab separated values (*.tsv);;"
			   "comma separated values (*.csv)"
				));

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(filters);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);

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
  if (recCheckBox->checkState()) 
    {
      attysScopeWindow->startRec();
      if (toneCheckBox->isChecked()) {
	      audiobeep->play();
      }
    } 
  else 
    {
      attysScopeWindow->stopRec();
      if (toneCheckBox->isChecked()) {
	      audiobeep->play();
      }
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
		s = QString::asprintf( "%d usec", tb_us);
	} else if (tb_us<1000000) {
		s = QString::asprintf( "%d msec", tb_us/1000);
	} else {
		s = QString::asprintf( "%d sec", tb_us/1000000);
	}		
	tbInfoLineEdit->setText(s);
	attysScopeWindow->setTB(tb_us);
}

void Attys_scope::resetTbEvent() {
	    attysScopeWindow->clearScreen();
};


void Attys_scope::slotStartUDP() {
	bool ok;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
					     tr("UDP Port #"),
					     QLineEdit::Normal,
					     "65000",
					     &ok);
	if (ok && !text.isEmpty()) {
		const int port = text.toInt();
		attysScopeWindow->startUDP(port);
	}
}

void Attys_scope::slotStopUDP() {
	attysScopeWindow->stopUDP();
};


void Attys_scope::specialChanged() {
	if (restartInfo) {
		QMessageBox msgBox;
		msgBox.setText("You need to restart Attys Scope that these changes take effect.");
		msgBox.exec();
		restartInfo = 0;
	}
};
