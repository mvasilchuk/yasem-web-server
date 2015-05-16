#include "httpproxyserver.h"
#include "macros.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QUrl>

using namespace yasem;

bool HttpProxyThread::use_thread_queue = true;
QQueue<HttpProxyThread*> HttpProxyThread::m_socket_thread_queue;

HttpProxyServer::HttpProxyServer(QObject *parent) :
    QTcpServer(parent),
    m_max_reqests(10)
{
    connect(&m_socket_timer, &QTimer::timeout, [=](){
        //DEBUG() << "timeout";
        if(!HttpProxyThread::m_socket_thread_queue.isEmpty())
        {
            HttpProxyThread::m_socket_thread_queue.dequeue()->m_paused = false;
            //if(HttpProxyThread::m_socket_thread_queue.isEmpty())
            //    stopTimer();
        }
    });
}

void HttpProxyServer::startTimer()
{
    DEBUG() << "trying to start timer";
    if(HttpProxyThread::use_thread_queue || m_max_reqests > 0)
    {
        m_socket_timer.setInterval(1000 / m_max_reqests);
        m_socket_timer.setSingleShot(false);
        m_socket_timer.start();
        DEBUG() << "timer started";
    }
}

void HttpProxyServer::stopTimer()
{
    DEBUG() << "trying to stop timer";
    if(m_socket_timer.isActive())
    {
        m_socket_timer.stop();
        DEBUG() << "timer stopped";
    }
}

void HttpProxyServer::startServer()
{
    startTimer();
    listen(QHostAddress::Any, 8089);
    qDebug() << "Proxy server running at port" << serverPort();
}

void HttpProxyServer::stopServer()
{
    if(isListening())
        close();
}

void HttpProxyServer::setMaxRequestsPerSecond(quint8 max_req)
{
    this->m_max_reqests = max_req;
    HttpProxyThread::use_thread_queue = (m_max_reqests == 0);
}

void HttpProxyServer::incomingConnection(qintptr handle)
{
    //DEBUG() << "incoming";
    //if(!m_socket_timer.isActive())
    //    startTimer();
    HttpProxyThread* thread = new HttpProxyThread(handle, this);
    thread->start();
}

HttpProxyThread::HttpProxyThread(qintptr ID, QObject *parent):
    QThread(parent),
    m_socket_descriptor(ID),
    m_paused(true)
{

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
    connect(m_proxy_socket, QTcpSocket::disconnected, [=](){
        DEBUG() << "close connection";
        if (m_proxy_socket) {
            if (m_socket)
                m_socket->disconnectFromHost();
            if (m_proxy_socket->error() != QTcpSocket::RemoteHostClosedError)
                qWarning() << "Error for:" << m_proxy_socket->errorString();
            m_proxy_socket->deleteLater();
        }
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
        m_socket->deleteLater();
    });
    connect(m_socket,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
             [=](QAbstractSocket::SocketError err){
        ERROR() << err;
    });

    // We'll have multiple clients, we want to know which is which
    //qDebug() << m_socket_descriptor << " http client connected";

    if(HttpProxyThread::use_thread_queue)
    {
        m_paused = true;
        HttpProxyThread::m_socket_thread_queue.enqueue(this);
    }

    exec();
}

void HttpProxyThread::processQuery()
{
    //DEBUG() << "process query";
    QByteArray requestData = m_socket->readAll();

    while(HttpProxyThread::use_thread_queue && m_paused)
    {
        //DEBUG() << "thread waiting" << m_socket->thread() << "of" << HttpProxyThread::m_socket_queue.count();
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
    requestLine = method + " " + req.toUtf8() + " " + version + "\r\n";
    requestData.prepend(requestLine);

    if(HttpProxyThread::use_thread_queue)
    {
        m_paused = true;
        HttpProxyThread::m_socket_thread_queue.enqueue(this);
    }

    if (m_proxy_socket->isOpen()) {
        m_proxy_socket->write(requestData);
    } else {
        m_proxy_socket->setProperty("requestData", requestData);

        //DEBUG() << "queue" << HttpProxyThread::m_socket_thread_queue.count();
        //DEBUG() << "Connecting to " << host << port;
        m_proxy_socket->connectToHost(host, port);
    }
}
