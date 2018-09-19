#-------------------------------------------------
#
# Project created by QtCreator 2018-08-22T15:35:39
#
#-------------------------------------------------

QT += core
QT += gui
QT += network
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CustomerClient
TEMPLATE = app

# DESTDIR
#
win32 {
	CONFIG(debug, debug|release): DESTDIR = ../../../bin/debug
	CONFIG(release, debug|release): DESTDIR = ../../../bin/release
}
unix {
	CONFIG(debug, debug|release): DESTDIR = ../../../bin_unix/debug
	CONFIG(release, debug|release): DESTDIR = ../../../bin_unix/release
}


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
	../../../lib/UdpSocket.cpp \
	../../../lib/SocketIO.cpp \
	../../../lib/Provider.cpp \
	../../../lib/XmlHelper.cpp \
	../../../lib/Order.cpp \
	main.cpp \
	MainWindow.cpp \
	ProviderView.cpp \
	ConfigSocket.cpp \
	OrderDialog.cpp \
	Options.cpp \
    OrderStateSocket.cpp \
    OptionsDialog.cpp


HEADERS += \
	../../../lib/UdpSocket.h \
	../../../lib/SocketIO.h \
	../../../lib/Provider.h \
	../../../lib/XmlHelper.h \
	../../../lib/Order.h \
	../../../lib/Crc32.h \
	MainWindow.h \
	ProviderView.h \
	ConfigSocket.h \
	OrderDialog.h \
	Options.h \
    OrderStateSocket.h \
    OptionsDialog.h


RESOURCES += \
    resources.qrc
