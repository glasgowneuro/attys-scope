#include "mainwindow.h"

#include <QtWidgets>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(int ignoreSettings)
	: attys_scope(new Attys_scope(this,ignoreSettings))
{
	setCentralWidget(attys_scope);

	QFile style(":/QTDark.stylesheet");
	style.open(QIODevice::ReadOnly);
	setStyleSheet(style.readAll());
	style.close();
	
        setAutoFillBackground(true);
	
	QMenu *fileMenu = menuBar()->addMenu(tr("&Config"));
	QAction *saveConfigAct = new QAction(tr("&Save"), this);
	fileMenu->addAction(saveConfigAct);
	connect(saveConfigAct,&QAction::triggered,attys_scope,&Attys_scope::slotSaveSettings);
	QAction *loadConfigAct = new QAction(tr("&Load"), this);
	fileMenu->addAction(loadConfigAct);
	connect(loadConfigAct,&QAction::triggered,attys_scope,&Attys_scope::slotLoadSettings);

	QMenu *pyMenu = menuBar()->addMenu(tr("&Python"));
	QAction *runPythonAct = new QAction(tr("&Run"), this);
	pyMenu->addAction(runPythonAct);
	connect(runPythonAct,&QAction::triggered,attys_scope,&Attys_scope::slotRunPython);

	QMenu *udpMenu = menuBar()->addMenu(tr("&Broadcast"));
	QAction *startUDPAct = new QAction(tr("&Start UDP"), this);
	udpMenu->addAction(startUDPAct);
	connect(startUDPAct,&QAction::triggered,attys_scope,&Attys_scope::slotStartUDP);

	QAction *stopUDPAct = new QAction(tr("&Stop UDP"), this);
	udpMenu->addAction(stopUDPAct);
	connect(stopUDPAct,&QAction::triggered,attys_scope,&Attys_scope::slotStopUDP);

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
	QAction *runGithub = new QAction(tr("&github"), this);
	helpMenu->addAction(runGithub);
	connect(runGithub,&QAction::triggered,this,&MainWindow::slotGithub);	
}

void MainWindow::slotGithub() {
	QDesktopServices::openUrl(QUrl("https://github.com/glasgowneuro/attys-scope")); 
}


/////////////////////////////////////////////////////////////////////



void helptxt(char *name) {
	fprintf(stderr,"Help: use '%s -i' or '%s /i' to disable reading the configuration.\n",name,name);
	exit(1);
}


int main( int argc, char **argv )
{
	for(int i = 0;i<argc;i++) {
		if (strstr(argv[i],"-h")) helptxt(argv[0]);
		if (strstr(argv[i],"--help")) helptxt(argv[0]);
	}

	// default values
	int ignoreSettings = 0;

	QSettings settings(QSettings::IniFormat, 
			   QSettings::UserScope,
			   ATTYS_STRING,
			   PROGRAM_NAME);

	QApplication a( argc, argv );		// create application object

	QPixmap pixmap(":/attys.png");
	QSplashScreen* splash = new QSplashScreen(pixmap);
	splash->setFont( QFont("Helvetica", 10, QFont::Black) );
	splash->show();
	splash->showMessage("Scanning for paired devices");
	a.processEvents();

	// callback
	AttysScanMsg attysScanMsg;
	attysScanMsg.splash = splash;
	attysScanMsg.app = &a;
	attysScan.registerCallback(&attysScanMsg);

	// see if we have any Attys!
	int ret = attysScan.scan(AttysScan::MAX_ATTYS_DEVS);

	// none detected
	if ((0 != ret) || (attysScan.getNAttysDevices()<1)) {
		char tmp[] = "No Attys detected.\nExiting.";
		attysScanMsg.message(0,tmp);
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		attysScan.unregisterCallback();
		delete splash;
		return -1;
	}

	for(int i = 0;i<argc;i++) {
		if (strstr(argv[i],"/i")) ignoreSettings = 1;	
		if (strstr(argv[i],"-i")) ignoreSettings = 1;
	}

	MainWindow mainWindow(ignoreSettings);

	// show widget
	mainWindow.show();
	attysScan.unregisterCallback();
	splash->finish(&mainWindow);
	delete splash;
	// run event loop
	return a.exec();
}


