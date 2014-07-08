#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T13:31:56
#
#-------------------------------------------------

VERSION = 1.0.0

QT       += network

QT       -= gui

CONFIG += c++11

TARGET = yasem-web-server
TEMPLATE = lib

DEFINES += WEBSERVER_LIBRARY

INCLUDEPATH += ../../yasem-core

SOURCES += webserver.cpp \
    serverthread.cpp \
    tcpserver.cpp \
    tcprequest.cpp \
    tcpresponse.cpp

HEADERS += webserver.h\
        webserver_global.h \
    serverthread.h \
    tcpserver.h \
    tcprequest.h \
    tcpresponse.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


include(../../common.pri)
DESTDIR = $$DEFAULT_PLUGIN_DIR

OTHER_FILES += \
    metadata.json \
    LICENSE \
    README.md
