#include "tcpserver.h"
#include "serverthread.h"
#include "core.h"

using namespace yasem;

TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent)
{
}

void TcpServer::startServer()
{
    int port = Core::instance()->settings()->value("web-server/port", 9999).toInt();

    if(!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server:" << this->errorString();
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

void TcpServer::stopServer()
{
    if(this->isListening())
        this->close();
    qDebug() << "Web server stopped";
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    //qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    ServerThread *thread = new ServerThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
