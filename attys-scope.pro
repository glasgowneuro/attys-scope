## Modify these variables:
win32 {
CONFIG += 64bit
TEMPLATE	= app
Release:CONFIG		+= qt release c++11
Debug:CONFIG		+= qt debug c++11
HEADERS		= attys-scope.h special.h current.h scopewindow.h gain.h highpass.h lowpass.h bandstop.h channel.h recbutton.h samplingrate.h audiobeep.h stylesheets.h mainwindow.h
SOURCES		= attys-scope.cpp special.cpp current.cpp scopewindow.cpp gain.cpp lowpass.cpp bandstop.cpp highpass.cpp channel.cpp recbutton.cpp samplingrate.cpp audiobeep.cpp mainwindow.cpp
TARGET		= attys-scope
INSTALLS        += target
QT             += widgets multimedia
QT += network
INCLUDEPATH     += /iir1
Debug:LIBS += \
	-L/iir1/Debug \
    -liir_static \
	-lWs2_32 \
	-L../attys-comm/Debug \
	-lattyscomm_static

Release:LIBS += \
	-L/iir1/Release \
    -liir_static \
	-lWs2_32 \
	-L../attys-comm/Release \
	-lattyscomm_static

RESOURCES     = application.qrc
RC_FILE = attysapp.rc
INCLUDEPATH += ../attys-comm
MOC_DIR = moc
OBJECTS_DIR = obj
}

unix:!macx {
TEMPLATE	= app
CONFIG		+= qt release c++11
HEADERS		= attys-scope.h scopewindow.h gain.h highpass.h lowpass.h channel.h special.h current.h bandstop.h recbutton.h samplingrate.h audiobeep.h stylesheets.h mainwindow.h
SOURCES		= attys-scope.cpp scopewindow.cpp gain.cpp lowpass.cpp highpass.cpp channel.cpp special.cpp current.cpp bandstop.cpp recbutton.cpp samplingrate.cpp audiobeep.cpp mainwindow.cpp
TARGET		= attys-scope
INSTALLS        += target
LIBS            += -liir -lattyscomm -lbluetooth
QT             	+= widgets
QT 		+= network
QT              += multimedia
target.path     = /usr/local/bin
RESOURCES       = application.qrc
}
