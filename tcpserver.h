#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSet>

namespace yasem {

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
   explicit TcpServer(QObject *parent = 0);
   virtual ~TcpServer(){
    }
   void startServer();
   void stopServer();
signals:

public slots:

protected:
   void incomingConnection(qintptr socketDescriptor);
   int m_max_connections;
   int m_connections_count;
   QSet<QThread*> m_thread_set;
   QThread* m_next_thread;

};

}

#endif // TCPSERVER_H
