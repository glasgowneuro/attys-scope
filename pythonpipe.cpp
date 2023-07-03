#include "pythonpipe.h"
#include <qobject.h>
#include <iostream>

PythonPipe::PythonPipe() : QObject() {
	connect(&qprocess,
		(void(QProcess::*)(int,QProcess::ExitStatus)) & QProcess::finished,
		[this](int,QProcess::ExitStatus) mutable {isRunning = false;});
	connect(&qprocess, &QProcess::readyReadStandardOutput, [this]() {
								       QString tmp(qprocess.readAllStandardOutput());
								       tmp = "{"+pythonScriptName.fileName()+"} " + tmp;
								       std::cout << tmp.toStdString();
								       pythonMessage(tmp);
							       });
	connect(&qprocess, &QProcess::readyReadStandardError, [this]() {
								      QString tmp(qprocess.readAllStandardError());
								      tmp = "["+pythonScriptName.fileName()+ "] " + tmp;
								      std::cout << tmp.toStdString();
								      pythonMessage(tmp);
							      });
	connect(&qprocess, &QProcess::errorOccurred, [this](QProcess::ProcessError e) {
		QString tmp;
		switch (e) {
		case QProcess::FailedToStart:
			tmp = "The process failed to start.";
			break;
		case QProcess::Crashed:
			tmp = "The process crashed after starting successfully.";
			break;
		case QProcess::Timedout:
			tmp = "The last waitFor...() function timed out. ";
			break;
		case QProcess::WriteError:
			tmp = "An error occurred when attempting to write to the process.";
			break;
		default:
			tmp = "An unknown error occurred.";
		}
		tmp = "[" + pythonScriptName.fileName() + "] Error: " + tmp + "\n";
		std::cout << tmp.toStdString();
		pythonMessage(tmp);
		});
}


int PythonPipe::start(QString filename) {
	pythonScriptName.setFile(filename);
	isRunning = true;
	QStringList arguments;
	arguments << filename;
	qprocess.start("python",arguments);
	return 0;
}

void PythonPipe::stop() {
	qprocess.close();
	isRunning = false;
}

void PythonPipe::write(const char* data)
{
	if (!isRunning) return;
	if (qprocess.state() == QProcess::Running) {
		qprocess.write(data);
	}
}
