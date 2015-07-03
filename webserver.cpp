#include "webserver.h"
#include "webserverpluginobject.h"
#include "httpproxyserver.h"

using namespace yasem;

WebServer::WebServer(QObject* parent):
    SDK::Plugin(parent)
{

}

WebServer::~WebServer()
{

}

void yasem::WebServer::register_dependencies()
{
    add_dependency({ SDK::ROLE_BROWSER, false });
}

void yasem::WebServer::register_roles()
{
    register_role(SDK::ROLE_WEB_SERVER, new WebServerPluginObject(this));
    register_role(SDK::ROLE_HTTP_PROXY, new HttpProxyServer(this));
}
