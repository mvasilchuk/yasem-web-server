#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T13:31:56
#
#-------------------------------------------------

QT       += network

QT       -= gui

CONFIG += C++11

TARGET = yasem-web-server
TEMPLATE = lib
CONFIG += plugin

DEFINES += WEBSERVER_LIBRARY

INCLUDEPATH += ../yasem-core

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


include(../common.pri)
DESTDIR = $$DEFAULT_PLUGIN_DIR

OTHER_FILES += \
    metadata.json \
    LICENSE \
    README.md
