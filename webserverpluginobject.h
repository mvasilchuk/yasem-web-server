#ifndef WEBSERVERPLUGINOBJECT_H
#define WEBSERVERPLUGINOBJECT_H

#include "webserverplugin.h"

#include <QObject>

namespace yasem
{
class TcpServer;
class WebServerPluginObject : public SDK::WebServerPlugin
{
    Q_OBJECT
public:
    explicit WebServerPluginObject(SDK::Plugin* plugin);
    virtual ~WebServerPluginObject();

signals:

public slots:
protected:
    TcpServer* m_tcp_server;

    // AbstractPluginObject interface
public:
    SDK::PluginObjectResult init();
    SDK::PluginObjectResult deinit();
};

}

#endif // WEBSERVERPLUGINOBJECT_H
