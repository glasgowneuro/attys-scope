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
#include <qsplashscreen.h>

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <qtextedit.h>
#include <qfont.h>


#include <iostream>
#include <fstream>

#include "scopewindow.h"
#include "attys_scope.h"

// version number
#define VERSION "1.0.0"

// config constants
#define SETTINGS_GLOBAL "global"
#define SETTINGS_CHANNELS "channelconfig"
#define CHSETTING_FORMAT "ch_mapping_dev%09d_ch%09d"
#define ATTYS_STRING "ATTYS"
#define PROGRAM_NAME "attys_scope"

class QCommentTextEdit : public QTextEdit {
protected:
	void keyPressEvent(QKeyEvent *event) {
		if ((event->key() != Qt::Key_Return)&&
		    (event->key() != Qt::Key_Enter)&&
		    (event->key() != Qt::Key_Tab)	)	{
			QTextEdit::keyPressEvent(event);
		}
	}
};


Attys_scope::Attys_scope(QWidget *parent,
	    QSplashScreen *_splash,
		int ignoreSettings,
		float notch,
		int num_of_devices,
		int csv,
		float lpFreq,
		float hpFreq
	)
    : QWidget( parent ) {

	splash = _splash;

    attysScopeWindow=new ScopeWindow(this,notch);

	int n_devs = attysScopeWindow->getNattysDevices();
	int channels = attysScopeWindow->getNchannels();

	tb_us = 1000000 / attysScopeWindow->getActualSamplingRate();

	// fonts
	voltageFont = new QFont("Courier",10);
	QFontMetrics voltageMetrics(*voltageFont);

	// this the main layout which contains two sub-windows:
	// the control window and the oscilloscope window
	mainLayout = new QHBoxLayout;
	mainLayout->setSpacing(0);
	setLayout(mainLayout);

	// the corresponding box which contains all the controls
	controlBox = new QGroupBox ();
	controlBox->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Minimum ) );
	controlBox->setAttribute(Qt::WA_DeleteOnClose, false);

	// now we create another layout which contains all the controls
	controlLayout = new QVBoxLayout(controlBox);
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
	allChGroup->setAttribute(Qt::WA_DeleteOnClose, false);
	allChLayout->setSpacing(0);
	allChGroup->setLayout(allChLayout);
	allChGroup->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						QSizePolicy::Expanding ) );

	int row = 1;

	channelLabel=new QPointer<QLabel>*[n_devs];
	channel=new QPointer<Channel>*[n_devs];
	voltageTextEdit=new QPointer<QTextEdit>*[n_devs];
	channelgrp=new QPointer<QGroupBox>*[n_devs];
	hbox=new QPointer<QHBoxLayout>*[n_devs];
	gain=new QPointer<Gain>*[n_devs];
	dcSub=new QPointer<DCSub>*[n_devs];
	hp=new QPointer<Hp>*[n_devs];
	lp=new QPointer<Lp>*[n_devs];
	subDClabel=new QPointer<QLabel>*[n_devs];
	hpLabel=new QPointer<QLabel>*[n_devs];
	lpLabel=new QPointer<QLabel>*[n_devs];
	
	// to the get the stuff a bit closer together
	char styleSheet[] = "padding:0px;margin:0px;border:0px;";

	QSettings settings(QSettings::IniFormat, 
			   QSettings::UserScope,
			   ATTYS_STRING,
			   PROGRAM_NAME);

	settings.beginGroup(SETTINGS_CHANNELS);

	AttysComm attysCommTmp(0);
	int nch_enabled = 0;
	for(int n=0;n<n_devs;n++) {
		channelLabel[n]=new QPointer<QLabel>[channels];
		channel[n]=new QPointer<Channel>[channels];
		voltageTextEdit[n]=new QPointer<QTextEdit>[channels];
		channelgrp[n]=new QPointer<QGroupBox>[channels];
		hbox[n]=new QPointer<QHBoxLayout>[channels];
		gain[n]=new QPointer<Gain>[channels];
		dcSub[n]=new QPointer<DCSub>[channels];
		subDClabel[n]=new QPointer<QLabel>[channels];
		hp[n]=new QPointer<Hp>[channels];
		hpLabel[n]= new QPointer<QLabel>[channels];
		lp[n]=new QPointer<Lp>[channels];
		lpLabel[n]= new QPointer<QLabel>[channels];

		allChLayout->addWidget(new QLabel(attysScopeWindow->getAttysName(n)), row, 1);
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
			char tmpCh[128];
			sprintf(tmpCh,CHSETTING_FORMAT,n,i);
			if (ignoreSettings) {
				channel[n][i] -> setChannel( i );
			} else {
				channel[n][i] -> setChannel(
					settings.value(tmpCh,i).toInt() );
			}
			if ( channel[n][i] -> isActive() )
				nch_enabled++;
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

			subDClabel[n][i] = new QLabel(" -DC");
			subDClabel[n][i]->setStyleSheet(styleSheet);
			subDClabel[n][i]->setFont(*voltageFont);
			hbox[n][i]->addWidget(subDClabel[n][i]);

			dcSub[n][i] = new DCSub((float)INERTIA_FOR_DC_DETECTION);
			dcSub[n][i]->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(dcSub[n][i]);

			hpLabel[n][i] = new QLabel(" HP");
			hpLabel[n][i]->setStyleSheet(styleSheet);
			hpLabel[n][i]->setFont(*voltageFont);
			hbox[n][i]->addWidget(hpLabel[n][i]);

			hp[n][i] = new Hp(attysScopeWindow->getActualSamplingRate(),
					  hpFreq);
			hp[n][i] ->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(hp[n][i]);

			lpLabel[n][i] = new QLabel(" LP");
			lpLabel[n][i]->setStyleSheet(styleSheet);
			lpLabel[n][i]->setFont(*voltageFont);
			hbox[n][i]->addWidget(lpLabel[n][i]);

			lp[n][i] = new Lp(attysScopeWindow->getActualSamplingRate(),
					  lpFreq);
			lp[n][i] ->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(lp[n][i]);

			gain[n][i] = new Gain();
			gain[n][i]->setStyleSheet(styleSheet);
			hbox[n][i]->addWidget(gain[n][i]);

			allChLayout->addWidget(channelgrp[n][i],row,1);
			row++;
		}
	}

	settings.endGroup();

	controlLayout->addWidget(allChScrollArea);
	allChScrollArea->setWidget(allChGroup);

	// at least one should be active not to make the user nervous.
	if (nch_enabled==0)
		channel[0][0]->setChannel( 0 );

	// now we create another layout which contains all the remaining controls
	restLayout = new QVBoxLayout;
	// the corresponding box which contains all the controls
	restGroup = new QGroupBox;
	restGroup->setAttribute(Qt::WA_DeleteOnClose, false);

	// notch filter
	// create a group for the notch filter
	notchGroupBox = new QGroupBox();
	notchGroupBox->setAttribute(Qt::WA_DeleteOnClose, false);
	notchGroupBox->setFlat(true);
	notchGroupBox->setStyleSheet(styleSheet);
	notchLayout = new QHBoxLayout();
	char tmp[128];
	sprintf(tmp,"notch");
	filterCheckbox=new QCheckBox( tmp );
	filterCheckbox->setChecked(false);
	notchLayout->addWidget(filterCheckbox);
	notchTextEdit=new QCommentTextEdit();
	commentFont = new QFont();
	QFontMetrics commentMetrics(*commentFont);
	notchTextEdit->setMaximumHeight ( commentMetrics.height() );
	notchTextEdit->setMaximumWidth ( 10*commentMetrics.width('X') );
	notchTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	notchTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	notchTextEdit->setFont(*commentFont);
	comment=new QLabel("Notch frequency (Hz):");
	notchLayout->addWidget(comment);
	notchLayout->addWidget(notchTextEdit);
       	notchGroupBox->setLayout(notchLayout);
	restLayout->addWidget(notchGroupBox);

	connect(notchTextEdit, SIGNAL(textChanged()),
		this, SLOT(notchFreqChanged()));

	// group for the record stuff
	recGroupBox = new QGroupBox();
	recGroupBox->setAttribute(Qt::WA_DeleteOnClose, false);
	recLayout = new QHBoxLayout();

	recLabel = new QLabel("Write to file:");
	recLayout->addWidget(recLabel);

	filePushButton = new QPushButton( "&filename" );
	filePushButton->setSizePolicy ( QSizePolicy(QSizePolicy::Fixed,
						    QSizePolicy::Fixed ));
	filePushButton->setStyleSheet(
		"background-color: white;border-style:outset;border-width: 2px;border-color: black;font: bold 14px; padding: 0px;");
	connect(filePushButton, SIGNAL( clicked() ),
		this, SLOT( enterFileName() ) );
	recLayout->addWidget(filePushButton);

	recPushButton = new QCheckBox( "&REC" );
	recPushButton->connect(recPushButton, SIGNAL( stateChanged(int) ),
			       this, SLOT( recstartstop(int) ) );
	recPushButton->setEnabled( false );
	recLayout->addWidget(recPushButton);

	recGroupBox->setLayout(recLayout);
	restLayout->addWidget(recGroupBox);


	// group for the time base
	tbgrp = new QGroupBox();
	tbgrp->setAttribute(Qt::WA_DeleteOnClose, false);
	tbLayout = new QHBoxLayout;
	tbFont = new QFont("Courier",12);
	tbFont->setBold(true);
	QFontMetrics tbMetrics(*tbFont);

	QLabel tbLabel("Timebase:");
	tbLayout->addWidget(&tbLabel);

	tbIncPushButton = new QPushButton( "slower" );

	char tbStyle[]="background-color: white;border-style:outset;border-width: 1px;border-color: black;font: bold 10px;padding: 6px;";
	tbIncPushButton->setStyleSheet(tbStyle);
	tbIncPushButton->setMaximumSize ( tbMetrics.width(" slower ") ,  
					  tbMetrics.height()*5/4 );
	tbIncPushButton->setFont(*tbFont);
	tbgrp->connect(tbIncPushButton, SIGNAL( clicked() ),
		this, SLOT( incTbEvent() ) );
	tbLayout->addWidget(tbIncPushButton);

	tbDecPushButton = new QPushButton( "faster" );
	tbDecPushButton->setStyleSheet(tbStyle);
	tbDecPushButton->setMaximumSize ( tbMetrics.width(" faster ") ,  
					  tbMetrics.height()*5/4 );
	tbDecPushButton->setFont(*tbFont);	
	tbgrp->connect(tbDecPushButton, SIGNAL( clicked() ),
		       this, SLOT( decTbEvent() ) );
	tbLayout->addWidget(tbDecPushButton);

	tbInfoTextEdit = new QTextEdit(tbgrp);
	tbInfoTextEdit->setFont (*tbFont);
	QFontMetrics metricsTb(*tbFont);
	tbInfoTextEdit->setMaximumHeight ( commentMetrics.height() * 1.5 );
	tbInfoTextEdit->setMaximumWidth ( commentMetrics.width('X') * 17 );
	tbInfoTextEdit->setReadOnly(true);
	tbInfoTextEdit->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	tbLayout->addWidget(tbInfoTextEdit);

	tbResetPushButton = new QPushButton( "clear" );
	tbResetPushButton->setStyleSheet("background-color: white;border-style:outset;border-width: 1px;border-color: black;font: bold 10px;padding: 6px;");
	tbResetPushButton->setMaximumSize ( tbMetrics.width("clear ") *10/9,  
					  tbMetrics.height() *11/9);
	tbResetPushButton->setFont(*tbFont);	
	tbgrp->connect(tbResetPushButton, SIGNAL( clicked() ),
		       this, SLOT( resetTbEvent() ) );
	tbLayout->addWidget(tbResetPushButton);

	tbgrp->setLayout(tbLayout);
	restLayout->addWidget(tbgrp);

	statusgrp = new QGroupBox;
	statusgrp->setAttribute(Qt::WA_DeleteOnClose, false);
	statusLayout = new QHBoxLayout;

	char status[256];
	sprintf(status,"# of devs: %d, Fs: %d Hz, lp: %1.1f Hz, hp: %1.1f Hz",
		attysScopeWindow->getNattysDevices(),
		attysScopeWindow->getActualSamplingRate(),
		lpFreq,hpFreq);
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

	attysScopeWindow->startDAQ();
}

Attys_scope::~Attys_scope() {
	QSettings settings(QSettings::IniFormat, 
			   QSettings::UserScope,
			   ATTYS_STRING,
			   PROGRAM_NAME);

	int n_devs = attysScopeWindow->getNattysDevices();
	int channels = attysScopeWindow->getNchannels();
	settings.beginGroup(SETTINGS_CHANNELS);
	for(int n=0;n<n_devs;n++) {
		for(int i=0;i<channels;i++) {
			char tmp[128];
			sprintf(tmp,CHSETTING_FORMAT,n,i);
			settings.setValue(tmp, 
					  channel[n][i] -> getChannel() );
		}
	}
	settings.endGroup();
}


void Attys_scope::disableControls() {
	filePushButton->setEnabled( false );
	int n_devs = attysScopeWindow->getNattysDevices();
	int channels = attysScopeWindow->getNchannels();
	for(int n=0;n<n_devs;n++) {
		for(int i=0;i<channels;i++) {
			channel[n][i]->setEnabled( false );
		}
	}
}


void Attys_scope::enableControls() {
	filePushButton->setEnabled( true );
	int n_devs = attysScopeWindow->getNattysDevices();
	int channels = attysScopeWindow->getNchannels();
	for(int n=0;n<n_devs;n++) {
		for(int i=0;i<channels;i++) {
			channel[n][i]->setEnabled( true );
		}
	}
}


void Attys_scope::notchFreqChanged() {
	QString str = notchTextEdit->toPlainText();
	attysScopeWindow->setNotchFrequency(str.toFloat());
}


void Attys_scope::setFilename(QString fn,int tsv) {
	attysScopeWindow->setFilename(fn,tsv);
	QString tmp;
	tmp="attys_scope - datafile: "+fn;
	setWindowTitle( tmp );
	recPushButton->setEnabled( true );
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
  if (recPushButton->checkState()==Qt::Checked) 
    {
      attysScopeWindow->startRec();
    } 
  else 
    {
      attysScopeWindow->stopRec();
      // to force the user to enter a new filename
      recPushButton->setEnabled( false );
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


int main( int argc, char **argv )
{
	// default values
	int num_of_devices = 16;
	float notch = 50;
	int tsv = 1;
	float lpFreq = 20;
	float hpFreq = 1;
	int ignoreSettings = 0;

	QSettings settings(QSettings::IniFormat, 
			   QSettings::UserScope,
			   ATTYS_STRING,
			   PROGRAM_NAME);

	QApplication a( argc, argv );		// create application object

	QPixmap pixmap(":/attys.png");
	QSplashScreen splash(pixmap);
	splash.show();
	splash.showMessage("Scanning for paired devices");

	for(int i = 0;i<argc;i++) {
		if (strstr(argv[i],"/i")) ignoreSettings = 1;	
		if (strstr(argv[i],"-i")) ignoreSettings = 1;
	}

	Attys_scope attys_scope(0,
		      &splash,
			  ignoreSettings,
			  notch,
			  num_of_devices,
			  tsv,
			  lpFreq,
			  hpFreq
		);

	// show widget
	attys_scope.show();
	splash.finish(&attys_scope);
	// run event loop
	return a.exec();
}
