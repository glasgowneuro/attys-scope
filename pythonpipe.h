#ifndef _PYTHONPIPE
#define _PYTHONPIPE

#include<stdio.h>
#include<qobject.h>
#include<qprocess.h>
#include<qdialog.h>
#include<qtextedit.h>
#include<QVBoxLayout>

class LogWindow : public QDialog {
public:
	LogWindow(QWidget* parent) : QDialog(parent) {
		QVBoxLayout *verticalLayout = new QVBoxLayout;
		textedit = new QTextEdit;
		verticalLayout->addWidget(textedit);
		setLayout(verticalLayout);
	}
	void addText(QString t) {
		textedit->append(t);
	}
private:
	QTextEdit *textedit;
};

class PythonPipe : public QObject {
	
	Q_OBJECT

public:
	PythonPipe() : QObject() {
		connect(&qprocess, (void(QProcess::*)(int)) & QProcess::finished, [this](int) mutable {isRunning = false;});
		connect(&qprocess, &QProcess::readyReadStandardOutput, [this]() {
									       QByteArray tmp = qprocess.readAllStandardOutput();
									       fprintf(stderr,"%s",tmp.constData());
									       pythonMessage(tmp);
								      });
		connect(&qprocess, &QProcess::readyReadStandardError, [this]() {
									       QByteArray tmp = qprocess.readAllStandardError();
									       fprintf(stderr,"%s",tmp.constData());
									       pythonMessage(tmp);
								      });
	}

	int start(QString theFilename);
	void stop();
	void write(const char* data);
	inline bool running() const {
		return isRunning;
	}
signals:
	void pythonMessage(QString s);
private:
	bool isRunning = false;
	QProcess qprocess;
};

#endif
