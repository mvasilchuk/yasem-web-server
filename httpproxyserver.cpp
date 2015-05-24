#include "httpproxyserver.h"
#include "macros.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QUrl>

using namespace yasem;

HttpProxyServer::HttpProxyServer(Plugin *plugin):
    AbstractHttpProxy(plugin),
    m_server(new HttpProxyTcpServer(this))
{
    connect(&m_socket_timer, &QTimer::timeout, [=](){
        //DEBUG() << "left queries" << m_socket_thread_queue.size();
        if(!m_socket_thread_queue.isEmpty())
        {
            m_socket_thread_queue.dequeue()->unpause();
        }
    });
}

void HttpProxyServer::startTimer()
{
    if(getMaxRequestPerSecond() > 0)
    {
        m_socket_timer.setInterval(1000 / getMaxRequestPerSecond());
        m_socket_timer.setSingleShot(false);
        m_socket_timer.start();
        DEBUG() << "timer started";
    }
}

void HttpProxyServer::stopTimer()
{
    if(m_socket_timer.isActive())
    {
        m_socket_timer.stop();
        DEBUG() << "timer stopped";
    }
}

bool HttpProxyServer::startServer()
{
    startTimer();
    setHostHame("127.0.0.1");
    setPort(8087);

    m_server->listen(QHostAddress(hostName()), port());
    qDebug() << qPrintable(QString("Proxy server running at %1:%2").arg(m_server->serverAddress().toString()).arg(m_server->serverPort()));
    return m_server->isListening();
}

bool HttpProxyServer::stopServer()
{
    if(m_server->isListening())
    {
        m_server->close();
        DEBUG() << "Http proxy stopped";
    }
    stopTimer();
    return true;
}


HttpProxyThread::HttpProxyThread(qintptr ID, HttpProxyServer *proxy_server):
    QThread(proxy_server),
    m_socket_descriptor(ID),
    m_proxy_server(proxy_server),
    m_paused(true)
{

}

bool HttpProxyThread::isPaused() const
{
    return m_paused;
}

void HttpProxyThread::pause()
{
    m_paused = true;
}

void HttpProxyThread::unpause()
{
    m_paused = false;
}

void HttpProxyThread::run()
{
    m_socket = new QTcpSocket();
    m_proxy_socket = new QTcpSocket();

    connect(m_proxy_socket, &QTcpSocket::connected, [=](){
        m_proxy_socket->write(m_proxy_socket->property("requestData").toByteArray());
    });
    connect(m_proxy_socket, &QTcpSocket::readyRead, [=](){
        m_socket->write(m_proxy_socket->readAll());
    });

    connect(m_proxy_socket,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
             [=](QAbstractSocket::SocketError err){
        ERROR() << err;
        m_proxy_socket->disconnectFromHost();
    });

    m_socket->moveToThread(this);
    m_proxy_socket->moveToThread(this);

    // set the ID
    if(!m_socket->setSocketDescriptor(this->m_socket_descriptor))
    {
        // something's wrong, we just emit a signal
        WARN() << m_socket->error();
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(m_socket, &QTcpSocket::readyRead, this, &HttpProxyThread::processQuery, Qt::DirectConnection);
    connect(m_socket, &QTcpSocket::connected, [=](){
        //DEBUG() << "socket connected" << m_socket->peerAddress();
    });
    connect(m_socket, &QTcpSocket::disconnected, [=](){
        //DEBUG() << "socket disconnected";

    });

    auto on_disconnect = [=](){
        DEBUG() << "close connection";
        if (m_socket != NULL)
        {
            if (m_socket->error() != QAbstractSocket::RemoteHostClosedError)
                WARN() << "Error for:" << m_socket->errorString();
            m_socket->disconnect();
            if(m_socket->state() == QAbstractSocket::ConnectedState)
                m_socket->disconnectFromHost();
            m_socket->deleteLater();
        }
        if (m_proxy_socket != NULL) {
            if (m_proxy_socket->error() != QAbstractSocket::RemoteHostClosedError)
                WARN() << "Error for:" << m_proxy_socket->errorString();
            m_proxy_socket->disconnect();
            if(m_proxy_socket->state() == QAbstractSocket::ConnectedState)
                m_proxy_socket->disconnectFromHost();

            m_proxy_socket->deleteLater();
        }
    };

    connect(m_socket, &QTcpSocket::disconnected, on_disconnect);
    connect(m_proxy_socket, &QTcpSocket::disconnected, on_disconnect);

    // We'll have multiple clients, we want to know which is which
    //qDebug() << m_socket_descriptor << " http client connected";

    if(m_proxy_server->getMaxRequestPerSecond() > 0)
    {
        this->pause();
    }
    m_proxy_server->addToQueue(this);

    exec();
}

void HttpProxyThread::processQuery()
{
    //DEBUG() << "process query";
    QByteArray requestData = m_socket->readAll();

    while(m_proxy_server->getMaxRequestPerSecond() > 0 && isPaused())
    {
        //DEBUG() << "thread waiting" << m_socket->thread() << "of" << m_proxy_server->queue().size();
        m_socket->thread()->msleep(1);
    }

    int pos = requestData.indexOf("\r\n");
    QByteArray requestLine = requestData.left(pos);
    requestData.remove(0, pos + 2);

    QList<QByteArray> entries = requestLine.split(' ');

    QByteArray method = entries.at(0);
    QByteArray address = entries.at(1);
    QByteArray version = entries.at(2);

    QUrl url = QUrl::fromEncoded(address);
    if (!url.isValid()) {
        qWarning() << "Invalid URL:" << url;
        m_socket->disconnectFromHost();
        return;
    }

    QString host = url.host();
    int port = (url.port() < 0) ? 80 : url.port();
    QString req = url.path();
    if (url.hasQuery())
        req.append('?').append(url.query());
    requestLine = method + " " + QUrl(req).toEncoded() + " " + version + "\r\n";
    requestData.prepend(requestLine);

    if (m_proxy_socket->isOpen()) {
        m_proxy_socket->write(requestData);
    } else {
        m_proxy_socket->setProperty("requestData", requestData);

        m_proxy_socket->connectToHost(host, port);
    }

    //if(m_proxy_server->getMaxRequestPerSecond() > 0)
    //{
    //    m_paused = true;
    //}
    //m_proxy_server->addToQueue(this);
}


PluginObjectResult yasem::HttpProxyServer::init()
{
    return PLUGIN_OBJECT_RESULT_OK;
}

PluginObjectResult yasem::HttpProxyServer::deinit()
{
    return PLUGIN_OBJECT_RESULT_OK;
}


bool yasem::HttpProxyServer::isRunning()
{
    return m_server->isListening();
}

QQueue<HttpProxyThread *> HttpProxyServer::getThreadQueue()
{
    return m_socket_thread_queue;
}

void HttpProxyServer::addToQueue(HttpProxyThread *socket_thread)
{
    m_socket_thread_queue.enqueue(socket_thread);
}

QQueue<HttpProxyThread *> HttpProxyServer::queue() const
{
    return m_socket_thread_queue;
}


void HttpProxyTcpServer::incomingConnection(qintptr handle)
{
    HttpProxyThread* thread = new HttpProxyThread(handle, static_cast<HttpProxyServer*>(parent()));
    thread->start();
}


HttpProxyTcpServer::HttpProxyTcpServer(QObject *parent):
    QTcpServer(parent)
{

}

HttpProxyTcpServer::~HttpProxyTcpServer()
{

}
