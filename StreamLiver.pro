#-------------------------------------------------
#
# Project created by QtCreator 2019-03-08T16:21:17
#
#-------------------------------------------------

QT       += core gui quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StreamLiver
TEMPLATE = app

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
        main.cpp \
        mainwindow.cpp \
    camerahelper.cpp \
    videorender.cpp

HEADERS += \
        mainwindow.h \
    camerahelper.h \
    videorender.h

DISTFILES +=

RESOURCES += \
    qml.qrc

INCLUDEPATH += C:\workspace\c++\obs-studio\plugins\win-dshow\libdshowcapture
LIBS += C:\workspace\c++\obs-studio\plugins\win-dshow\libdshowcapture\build\Debug\libdshowcapture.lib

INCLUDEPATH += C:\workspace\c++\SDL\SDL2-devel-2.0.9-VC\SDL2-2.0.9\include
LIBS += C:\workspace\c++\SDL\SDL2-devel-2.0.9-VC\SDL2-2.0.9\lib\x64\SDL2.lib
