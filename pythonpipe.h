#pragma once

#include<stdio.h>
#include<qobject.h>
#include<qprocess.h>

class PythonPipe {

public:
	int start(QString theFilename, QObject* parent = nullptr);
	void stop();
	void write(const char* data);
	inline bool hasError() const {
		return ((qprocess->error() < QProcess::UnknownError));
	}
	bool processFinished = false;
private:
	QProcess* qprocess = nullptr;
};
