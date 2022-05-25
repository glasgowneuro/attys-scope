#include "pythonpipe.h"
#include <qobject.h>
#include <iostream>

PythonPipe::PythonPipe() : QObject() {
	connect(&qprocess, (void(QProcess::*)(int)) & QProcess::finished, [this](int) mutable {isRunning = false;});
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
}


int PythonPipe::start(QString filename) {
	pythonScriptName.setFile(filename);
	filename = filename;
	filename = "python " + filename;
	isRunning = true;
	qprocess.start(filename);
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
