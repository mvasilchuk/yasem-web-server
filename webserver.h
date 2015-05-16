#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "stbpluginobject.h"
#include "tcpserver.h"
#include "webserverplugin.h"

#include "serverthread.h"

#include "webserver_global.h"
#include <QTcpServer>

namespace yasem {

class WEBSERVERSHARED_EXPORT WebServer:  public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.WebServer/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::Plugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "Web server plugin for YASEM")
public:
    explicit WebServer(QObject* parent = 0);
    virtual ~WebServer();

public:
    void register_dependencies();
    void register_roles();
};

}

#endif // WEBSERVER_H
