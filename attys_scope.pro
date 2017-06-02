## Modify these variables:
win32 {
TEMPLATE	= app
CONFIG		+= qt debug
HEADERS		= attys_scope.h AttysComm.h special.h current.h scopewindow.h gain.h dc_sub.h hp.h lp.h channel.h base64.h iir1/iir.h iir1/iir/Common.h iir1/iir/Bessel.h iir1/iir/Biquad.h iir1/iir/Butterworth.h iir1/iir/Cascade.h iir1/iir/ChebyshevI.h iir1/iir/ChebyshevII.h iir1/iir/Custom.h iir1/iir/Elliptic.h iir1/iir/Legendre.h iir1/iir/PoleFilter.h iir1/iir/RBJ.h iir1/iir/RootFinder.h iir1/iir/State.cpp
SOURCES		= attys_scope.cpp AttysComm.cpp special.cpp current.cpp scopewindow.cpp gain.cpp dc_sub.cpp lp.cpp hp.cpp channel.cpp base64.c iir1/iir/Bessel.cpp iir1/iir/Biquad.cpp iir1/iir/Butterworth.cpp iir1/iir/Cascade.cpp iir1/iir/ChebyshevI.cpp iir1/iir/ChebyshevII.cpp iir1/iir/Custom.cpp iir1/iir/Elliptic.cpp iir1/iir/Legendre.cpp iir1/iir/PoleFilter.cpp iir1/iir/RBJ.cpp iir1/iir/RootFinder.cpp iir1/iir/State.cpp
TARGET		= attys_scope
INSTALLS        += target
QT             += widgets
INCLUDEPATH     += iir1
RESOURCES     = application.qrc
RC_FILE = attysapp.rc
}

unix {
TEMPLATE	= app
CONFIG		+= qt debug c++11
HEADERS		= attys_scope.h AttysComm.h scopewindow.h gain.h dc_sub.h hp.h lp.h channel.h base64.h
SOURCES		= attys_scope.cpp AttysComm.cpp scopewindow.cpp gain.cpp dc_sub.cpp lp.cpp hp.cpp channel.cpp base64.c
TARGET		= attys_scope
INSTALLS        += target
LIBS            += -liir -lbluetooth
QT             += widgets
target.path     = /usr/local/bin
RESOURCES     = application.qrc
}
