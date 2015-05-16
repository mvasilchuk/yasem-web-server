#include "webserver.h"
#include "webserverpluginobject.h"

using namespace yasem;

WebServer::WebServer(QObject* parent):
    Plugin(parent)
{

}

WebServer::~WebServer()
{

}

void yasem::WebServer::register_dependencies()
{
    add_dependency(PluginDependency(ROLE_BROWSER, false));
}

void yasem::WebServer::register_roles()
{
    register_role(ROLE_WEB_SERVER, new WebServerPluginObject(this));
}
