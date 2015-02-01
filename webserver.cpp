#include "webserver.h"
#include "tcpserver.h"


using namespace yasem;

WebServer::WebServer()
{
    tcpServer = new TcpServer(this);
}

PLUGIN_ERROR_CODES WebServer::initialize()
{
    tcpServer->startServer();
    return PLUGIN_ERROR_NO_ERROR;
}

PLUGIN_ERROR_CODES WebServer::deinitialize()
{
    tcpServer->stopServer();
    return PLUGIN_ERROR_NO_ERROR;
}

void yasem::WebServer::register_dependencies()
{
    add_dependency(PluginDependency(ROLE_BROWSER, false));
}

void yasem::WebServer::register_roles()
{
    register_role(ROLE_WEB_SERVER);
}
