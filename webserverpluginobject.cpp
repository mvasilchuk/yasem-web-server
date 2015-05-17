#include "webserverpluginobject.h"

#include "tcpserver.h"

using namespace yasem;

WebServerPluginObject::WebServerPluginObject(Plugin* plugin) :
    WebServerPlugin(plugin),
    m_tcp_server(new TcpServer(this))
{

}

WebServerPluginObject::~WebServerPluginObject()
{

}

PluginObjectResult WebServerPluginObject::init()
{
    m_tcp_server->startServer();
    return PLUGIN_OBJECT_RESULT_OK;
}

PluginObjectResult WebServerPluginObject::deinit()
{
    m_tcp_server->stopServer();
    return PLUGIN_OBJECT_RESULT_OK;
}

