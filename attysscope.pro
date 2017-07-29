## Modify these variables:
win32 {
TEMPLATE	= app
CONFIG		+= qt release
HEADERS		= attysscope.h ../AttysComm/c/AttysComm.h  ../AttysComm/c/AttysScan.h special.h current.h scopewindow.h gain.h highpass.h lowpass.h bandstop.h channel.h ../AttysComm/c/base64.h iir1/iir.h iir1/iir/Common.h iir1/iir/Bessel.h iir1/iir/Biquad.h iir1/iir/Butterworth.h iir1/iir/Cascade.h iir1/iir/ChebyshevI.h iir1/iir/ChebyshevII.h iir1/iir/Custom.h iir1/iir/Elliptic.h iir1/iir/Legendre.h iir1/iir/PoleFilter.h iir1/iir/RBJ.h iir1/iir/RootFinder.h iir1/iir/State.cpp
SOURCES		= attysscope.cpp ../AttysComm/c/AttysComm.cpp ../AttysComm/c/AttysScan.cpp special.cpp current.cpp scopewindow.cpp gain.cpp lowpass.cpp bandstop.cpp highpass.cpp channel.cpp ../AttysComm/c/base64.c iir1/iir/Bessel.cpp iir1/iir/Biquad.cpp iir1/iir/Butterworth.cpp iir1/iir/Cascade.cpp iir1/iir/ChebyshevI.cpp iir1/iir/ChebyshevII.cpp iir1/iir/Custom.cpp iir1/iir/Elliptic.cpp iir1/iir/Legendre.cpp iir1/iir/PoleFilter.cpp iir1/iir/RBJ.cpp iir1/iir/RootFinder.cpp iir1/iir/State.cpp
TARGET		= Attys_Scope
INSTALLS        += target
QT             += widgets
QT += network
INCLUDEPATH     += iir1
RESOURCES     = application.qrc
RC_FILE = attysapp.rc
INCLUDEPATH += ../AttysComm/c/
}

unix {
TEMPLATE	= app
CONFIG		+= qt release c++11
HEADERS		= attysscope.h ../AttysComm/c/AttysComm.h ../AttysComm/c/AttysScan.h scopewindow.h gain.h highpass.h lowpass.h channel.h ../AttysComm/c/base64.h special.h current.h bandstop.h 
SOURCES		= attysscope.cpp ../AttysComm/c/AttysComm.cpp ../AttysComm/c/AttysScan.cpp scopewindow.cpp gain.cpp lowpass.cpp highpass.cpp channel.cpp ../AttysComm/c/base64.c special.cpp current.cpp bandstop.cpp 
TARGET		= attysscope
INSTALLS        += target
LIBS            += -liir -lbluetooth
QT             	+= widgets
QT 		+= network
target.path     = /usr/local/bin
RESOURCES       = application.qrc
INCLUDEPATH     += ../AttysComm/c/
}
