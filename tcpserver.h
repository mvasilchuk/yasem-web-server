#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

namespace yasem {

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
   explicit TcpServer(QObject *parent = 0);
   virtual ~TcpServer(){
        stopServer();
    }
   void startServer();
   void stopServer();
signals:

public slots:

protected:
   void incomingConnection(qintptr socketDescriptor);

};

}

#endif // TCPSERVER_H
