#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "stbplugin.h"
#include "tcpserver.h"

#include "serverthread.h"

#include "webserver_global.h"
#include <QTcpServer>

namespace yasem {

class WEBSERVERSHARED_EXPORT WebServer: public QObject, public virtual Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.WebServer/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::Plugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "Web server plugin for YASEM")
public:
    WebServer();

    TcpServer server;

    // Plugin interface
public:
    PLUGIN_ERROR_CODES initialize();
    PLUGIN_ERROR_CODES deinitialize();
};

}

#endif // WEBSERVER_H
