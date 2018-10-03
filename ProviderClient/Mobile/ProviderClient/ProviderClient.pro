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

win32 {
	TARGET = ProviderClient
}
android{
	TARGET = BookingResto
}
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
	OrderReceiveSocket.cpp \
    ../../../lib/Crc32.cpp \
    CancelOrderDialog.cpp


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
	OrderReceiveSocket.h \
    ../../../lib/Crc32.h \
    CancelOrderDialog.h


RESOURCES += \
	resources.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-xxhdpi/icon.png

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
