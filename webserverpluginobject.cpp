#include "webserverpluginobject.h"

#include "tcpserver.h"
#include "httpproxyserver.h"

using namespace yasem;

WebServerPluginObject::WebServerPluginObject(Plugin* plugin) :
    WebServerPlugin(plugin),
    m_tcp_server(new TcpServer(this)),
    m_http_proxy(new HttpProxyServer(this))
{

}

WebServerPluginObject::~WebServerPluginObject()
{

}

PluginObjectResult WebServerPluginObject::init()
{
    m_tcp_server->startServer();
    m_http_proxy->startServer();
    return PLUGIN_OBJECT_RESULT_OK;
}

PluginObjectResult WebServerPluginObject::deinit()
{
    m_tcp_server->stopServer();
    m_http_proxy->stopServer();
    return PLUGIN_OBJECT_RESULT_OK;
}

