#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H


#include <QThread>
#include <QTcpSocket>
#include <QHash>
#include <QMimeDatabase>

namespace yasem {

class BrowserPlugin;
class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(qintptr ID, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

protected:

    QTcpSocket *socket;
    qintptr socketDescriptor;
    BrowserPlugin* browser;
    QString rootDir;
    QMimeDatabase mimeDatabase;


};

}

#endif // SERVERTHREAD_H
