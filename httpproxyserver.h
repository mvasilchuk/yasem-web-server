#ifndef HTTPPROXYSERVER_H
#define HTTPPROXYSERVER_H

#include <QObject>
#include <QThread>
#include <qtcpserver.h>
#include <QQueue>
#include <QTimer>

class QTcpServer;
class QTcpSocket;

namespace yasem {

class HttpProxyServer;
class HttpProxyThread: public QThread
{
    Q_OBJECT
public:
    explicit HttpProxyThread(qintptr ID, QObject *parent = 0);
    // QThread interface
protected:
    void run();

private slots:
    void processQuery();

protected:
    QTcpSocket* m_socket;
    QTcpSocket* m_proxy_socket;
    qintptr m_socket_descriptor;

    static bool use_thread_queue;
    static QQueue<HttpProxyThread*> m_socket_thread_queue;

    volatile bool m_paused;

    friend class HttpProxyServer;
};

class HttpProxyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HttpProxyServer(QObject *parent = 0);

signals:

public slots:
    void startTimer();
    void stopTimer();
    void startServer();
    void stopServer();
    void setMaxRequestsPerSecond(quint8 max_req);

    // QTcpServer interface
protected:

    quint8 m_max_reqests;
    QTimer m_socket_timer;
    void incomingConnection(qintptr handle);
};

}

#endif // HTTPPROXYSERVER_H
