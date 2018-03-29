## Modify these variables:
win32 {
TEMPLATE	= app
CONFIG		+= qt release
HEADERS		= attys-scope.h ../AttysComm/c/AttysComm.h  ../AttysComm/c/AttysScan.h special.h current.h scopewindow.h gain.h highpass.h lowpass.h bandstop.h channel.h ../AttysComm/c/base64.h 
SOURCES		= attys-scope.cpp ../AttysComm/c/AttysComm.cpp ../AttysComm/c/AttysScan.cpp special.cpp current.cpp scopewindow.cpp gain.cpp lowpass.cpp bandstop.cpp highpass.cpp channel.cpp ../AttysComm/c/base64.c 
TARGET		= attys-scope
INSTALLS        += target
QT             += widgets
QT += network
INCLUDEPATH     += /iir1
LIBS += \
	-L/iir1/Debug \
    -liir_static \
	-lws2_32

RESOURCES     = application.qrc
RC_FILE = attysapp.rc
INCLUDEPATH += ../AttysComm/c/
}

unix {
TEMPLATE	= app
CONFIG		+= qt release c++11
HEADERS		= attys-scope.h ../AttysComm/c/AttysComm.h ../AttysComm/c/AttysScan.h scopewindow.h gain.h highpass.h lowpass.h channel.h ../AttysComm/c/base64.h special.h current.h bandstop.h 
SOURCES		= attys-scope.cpp ../AttysComm/c/AttysComm.cpp ../AttysComm/c/AttysScan.cpp scopewindow.cpp gain.cpp lowpass.cpp highpass.cpp channel.cpp ../AttysComm/c/base64.c special.cpp current.cpp bandstop.cpp 
TARGET		= attys-scope
INSTALLS        += target
LIBS            += -liir -lbluetooth
QT             	+= widgets
QT 		+= network
target.path     = /usr/local/bin
RESOURCES       = application.qrc
INCLUDEPATH     += ../AttysComm/c/
}
