#include "pythonpipe.h"
#include <qobject.h>

int PythonPipe::start(QString filename) {
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
