## Modify these variables:
TEMPLATE	= app
CONFIG		+= qt debug
HEADERS		= attys_scope.h comediscope.h gain.h ext_data_receive.h dc_sub.h hp.h lp.h fftscope.h channel.h base64.h
SOURCES		= attys_scope.cpp comediscope.cpp gain.cpp ext_data_receive.cpp dc_sub.cpp lp.cpp hp.cpp fftscope.cpp channel.cpp base64.c
TARGET		= attys_scope
LIBS            += -liir -lqwt-qt5 -lfftw3 -lbluetooth
target.path     = /usr/local/bin
INSTALLS        += target
ICON		= attys_scope.svg
QT             += widgets
