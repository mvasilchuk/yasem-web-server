#include "webserverpluginobject.h"

#include "tcpserver.h"

using namespace yasem;

WebServerPluginObject::WebServerPluginObject(SDK::Plugin* plugin) :
    WebServerPlugin(plugin),
    m_tcp_server(new TcpServer(this))
{

}

WebServerPluginObject::~WebServerPluginObject()
{

}

SDK::PluginObjectResult WebServerPluginObject::init()
{
    m_tcp_server->startServer();
    return SDK::PLUGIN_OBJECT_RESULT_OK;
}

SDK::PluginObjectResult WebServerPluginObject::deinit()
{
    m_tcp_server->stopServer();
    return SDK::PLUGIN_OBJECT_RESULT_OK;
}

