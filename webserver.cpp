#include "webserver.h"
#include "tcpserver.h"


using namespace yasem;

WebServer::WebServer()
{

}

PLUGIN_ERROR_CODES WebServer::initialize()
{
    server.startServer();
    return PLUGIN_ERROR_NO_ERROR;
}

PLUGIN_ERROR_CODES WebServer::deinitialize()
{
    server.stopServer();
    return PLUGIN_ERROR_NO_ERROR;
}

void yasem::WebServer::register_dependencies()
{
    add_dependency(PluginDependency(ROLE_BROWSER, false));
}

void yasem::WebServer::register_roles()
{
    register_role(ROLE_UNSPECIFIED);
}
