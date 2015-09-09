#ifndef HTTPPROXYSERVER_H
#define HTTPPROXYSERVER_H

#include "abstracthttpproxy.h"

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>

namespace yasem {

class HttpProxyServer;

class HttpProxyTcpServer: public QTcpServer
{
    Q_OBJECT
public:
    HttpProxyTcpServer(QObject* parent = 0);
    virtual ~HttpProxyTcpServer();

    // QTcpServer interface
protected:
    void incomingConnection(qintptr handle);
};

class HttpProxyThread: public QThread
{
    Q_OBJECT
public:
    explicit HttpProxyThread(qintptr ID, HttpProxyServer *proxy_server);
    virtual ~HttpProxyThread();
    // QThread interface

    bool isPaused() const;
    void pause();
    void unpause();

protected:
    void run();

private slots:
    void processQuery();
    void onDisconnected();

protected:
    qintptr m_socket_descriptor;
    QPointer<QTcpSocket> m_socket;
    QPointer<QTcpSocket> m_proxy_socket;
    HttpProxyServer* m_proxy_server;

    volatile bool m_paused;
};

class HttpProxyServer: public SDK::HttpProxy
{
    Q_OBJECT
public:
    explicit HttpProxyServer(SDK::Plugin *plugin);
    virtual ~HttpProxyServer();

signals:

public slots:
    void startTimer();
    void stopTimer();

    // QTcpServer interface
protected:

    HttpProxyTcpServer* m_server;
    QTimer m_socket_timer;
    QQueue<HttpProxyThread*> m_socket_thread_queue;

    // AbstractPluginObject interface
public:
    SDK::PluginObjectResult init();
    SDK::PluginObjectResult deinit();

    // AbstractHttpProxy interface
public:
    bool startServer();
    bool stopServer();
    bool isRunning();
    QQueue<HttpProxyThread*> getThreadQueue();
    void addToQueue(HttpProxyThread* socket_thread);
    QQueue<HttpProxyThread*> queue() const;

};

}

#endif // HTTPPROXYSERVER_H
