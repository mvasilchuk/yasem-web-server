#include "tcpserver.h"
#include "serverthread.h"
#include "core.h"
#include "macros.h"

using namespace yasem;

TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent),
    m_max_connections(1),
    m_connections_count(0),
    m_next_thread(NULL)
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
    DEBUG() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    ServerThread *thread = new ServerThread(socketDescriptor, this);
    m_thread_set.insert(thread);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, &QThread::finished, [=](){
        DEBUG() << "thread finished";
        DEBUG() << "current connections " << m_connections_count;
        thread->deleteLater();
        m_connections_count--;
        m_thread_set.remove(thread);
        DEBUG() << "Threads left in queue" << m_thread_set.count();
        if(m_connections_count < m_max_connections)
        {
            if(!m_thread_set.isEmpty())
                m_next_thread = m_thread_set.values().first();
            else
                m_next_thread = NULL;
        }
    });

    thread->start();

    if(m_next_thread == NULL || m_connections_count < m_max_connections)
    {
        m_next_thread = thread;
    }

    while(m_next_thread != thread)
    {
        DEBUG() << "Sleepping";
        thread->msleep(100);
    }

    m_connections_count++;
}
