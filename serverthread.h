#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H


#include <QThread>
#include <QTcpSocket>
#include <QHash>
#include <QMimeDatabase>
#include <QSharedPointer>

namespace yasem {

namespace SDK {
class Browser;
class WebServerPlugin;
}

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(qintptr ID, QObject *parent = 0);
    virtual ~ServerThread();

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

protected:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    QSharedPointer<SDK::Browser> browser;
    QMimeDatabase mimeDatabase;
    SDK::WebServerPlugin* webServer;
};

}

#endif // SERVERTHREAD_H
