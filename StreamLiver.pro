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
    camerahelper.cpp \
    main.cpp \
    mainwindow.cpp \
    videoencoder.cpp \
    videorender.cpp \    
    liver.cpp \
    librtmp/amf.c \
    librtmp/cencode.c \
    librtmp/hashswf.c \
    librtmp/log.c \
    librtmp/md5.c \
    librtmp/parseurl.c \
    librtmp/platform.c \
    librtmp/rtmp.c \
    flv-mux.c \
    bmem.c \
    base.c \
    array-serializer.c

HEADERS += \
    mainwindow.h \
    camerahelper.h \
    videorender.h \
    videoencoder.h \
    liver.h \    
    librtmp/amf.h \
    librtmp/bytes.h \
    librtmp/c99defs.h \
    librtmp/cencode.h \
    librtmp/dh.h \
    librtmp/dhgroups.h \
    librtmp/handshake.h \
    librtmp/http.h \
    librtmp/log.h \
    librtmp/md5.h \
    librtmp/platform.h \
    librtmp/rtmp.h \
    librtmp/rtmp_sys.h \
    flv-mux.h \
    rtmp-helpers.h \
    array-serializer.h \
    darray.h \
    serializer.h \
    c99defs.h \
    bmem.h \
    base.h \
    common_def.h \
    metadata_def.h \
    serialize_buf.h

# DISTFILES += \

RESOURCES += \
    qml.qrc

INCLUDEPATH += C:\workspace\c++\obs-studio\plugins\win-dshow\libdshowcapture
LIBS += C:\workspace\c++\obs-studio\plugins\win-dshow\libdshowcapture\build\Debug\libdshowcapture.lib

INCLUDEPATH += C:\workspace\c++\SDL\SDL2-devel-2.0.9-VC\SDL2-2.0.9\include
LIBS += C:\workspace\c++\SDL\SDL2-devel-2.0.9-VC\SDL2-2.0.9\lib\x64\SDL2.lib

INCLUDEPATH += C:\workspace\c++\ffmpeg\ffmpeg-4.1.1-win64-dev\include
LIBS += C:\workspace\c++\ffmpeg\ffmpeg-4.1.1-win64-dev\lib\avcodec.lib
LIBS += C:\workspace\c++\ffmpeg\ffmpeg-4.1.1-win64-dev\lib\avdevice.lib
LIBS += C:\workspace\c++\ffmpeg\ffmpeg-4.1.1-win64-dev\lib\avformat.lib
LIBS += C:\workspace\c++\ffmpeg\ffmpeg-4.1.1-win64-dev\lib\avformat.lib
LIBS += C:\workspace\c++\ffmpeg\ffmpeg-4.1.1-win64-dev\lib\avutil.lib
LIBS += C:\workspace\c++\ffmpeg\ffmpeg-4.1.1-win64-dev\lib\swscale.lib

INCLUDEPATH += C:\workspace\openssl\openssl-1.0.2r\inc32
LIBS += C:\workspace\openssl\openssl-1.0.2r\out32dll\libeay32.lib
LIBS += C:\workspace\openssl\openssl-1.0.2r\out32dll\ssleay32.lib

INCLUDEPATH += C:\workspace\thirdparty\zlib\zlib-1.2.11
LIBS += C:\workspace\thirdparty\zlib\zlib-1.2.11\zlib1.lib
