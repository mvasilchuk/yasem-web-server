#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "plugin.h"

#include "webserver_global.h"
#include <QTcpServer>

namespace yasem {

class WEBSERVERSHARED_EXPORT WebServer:  public SDK::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mvas.yasem.WebServer/1.0" FILE "metadata.json")
    Q_INTERFACES(yasem::SDK::Plugin)

    Q_CLASSINFO("author", "Maxim Vasilchuk")
    Q_CLASSINFO("description", "Simple web server")
    Q_CLASSINFO("version", MODULE_VERSION)
    Q_CLASSINFO("revision", GIT_VERSION)
public:
    explicit WebServer(QObject* parent = 0);
    virtual ~WebServer();

public:
    void register_dependencies();
    void register_roles();
};

}

#endif // WEBSERVER_H
