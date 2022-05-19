#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>

#include "attys-scope.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int ignoreSettings = 0);

private:
    Attys_scope* attys_scope;
};

#endif
