#ifndef TCPREQUEST_H
#define TCPREQUEST_H

#include <QObject>

namespace yasem
{

class TcpRequest : public QObject
{
    Q_OBJECT
public:
    explicit TcpRequest(QObject *parent = 0);

signals:

public slots:

};

}

#endif // TCPREQUEST_H
