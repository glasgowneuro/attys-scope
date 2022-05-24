#include "pythonpipe.h"
#include <qobject.h>

int PythonPipe::start(QString filename, QObject* parent) {
	filename = filename;
	filename = "python " + filename;
	processFinished = false;
    qprocess = new QProcess(parent);
	qprocess->connect(qprocess, (void(QProcess::*)(int)) & QProcess::finished, [=](int) mutable {processFinished=true;});
	qprocess->start(filename);
	return 0;
}

void PythonPipe::stop() {
    qprocess->close();
	delete qprocess;
}

void PythonPipe::write(const char* data)
{
	if (qprocess->state() == QProcess::Running) {
		qprocess->write(data);
	}
}

