#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T13:31:56
#
#-------------------------------------------------


VERSION = 0.1.0
TARGET = yasem-web-server
TEMPLATE = lib

include($${top_srcdir}/common.pri)

QT       += network

QT       -= gui

DEFINES += WEBSERVER_LIBRARY

SOURCES += webserver.cpp \
    serverthread.cpp \
    tcpserver.cpp \
    tcpresponse.cpp \
    $${CORE_ROOT_DIR}/webserverplugin.cpp \
    webserverpluginobject.cpp \
    httpproxyserver.cpp

HEADERS += webserver.h\
        webserver_global.h \
    serverthread.h \
    tcpserver.h \
    tcpresponse.h \
    webserverpluginobject.h \
    httpproxyserver.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    metadata.json \
    LICENSE \
    README.md
