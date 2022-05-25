#ifndef _PYTHONPIPE
#define _PYTHONPIPE

#include<stdio.h>
#include<qobject.h>
#include<qprocess.h>
#include<qdialog.h>
#include<qtextedit.h>
#include<QVBoxLayout>
#include<QFileInfo>


class PythonPipe : public QObject {
	
	Q_OBJECT

public:
	PythonPipe();
	int start(QString theFilename);
	void stop();
	void write(const char* data);
	inline bool running() const {
		return isRunning;
	}
	~PythonPipe() {
		stop();	
	}
signals:
	void pythonMessage(QString s);
private:
	bool isRunning = false;
	QProcess qprocess;
	QFileInfo pythonScriptName;
};




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








#endif
