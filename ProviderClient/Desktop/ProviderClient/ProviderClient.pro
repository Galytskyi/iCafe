#-------------------------------------------------
#
# Project created by QtCreator 2018-08-29T13:29:09
#
#-------------------------------------------------

QT += core
QT += gui
QT += network
QT += xml
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProviderClient
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

# include(../../../qtpropertybrowser/src/qtpropertybrowser.pri)

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
	../../../lib/Order.cpp \
	../../../lib/Provider.cpp \
	../../../lib/XmlHelper.cpp \
	../../../lib/UdpSocket.cpp \
	../../../lib/SocketIO.cpp \
	main.cpp \
	MainWindow.cpp \
	Options.cpp \
	OrderView.cpp \
	OptionsDialog.cpp \
	Database.cpp \
	OrderStateSocket.cpp \
	OrderReceiveSocket.cpp


HEADERS += \
	MainWindow.h \
	Options.h \
	../../../lib/Order.h \
	../../../lib/SocketIO.h \
	../../../lib/Provider.h \
	../../../lib/XmlHelper.h \
	../../../lib/UdpSocket.h \
	OrderView.h \
	OptionsDialog.h \
	Database.h \
	OrderStateSocket.h \
	OrderReceiveSocket.h


RESOURCES += \
	resources.qrc
