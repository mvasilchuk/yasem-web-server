#ifndef TCPRESPONSE_H
#define TCPRESPONSE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QHash>

namespace yasem
{

class TcpResponse : public QObject
{
    Q_OBJECT
public:
    explicit TcpResponse(QObject *parent = 0);
    virtual ~TcpResponse();



protected:
    int status;
    QHash<QString, QString> headers;
    QByteArray data;

signals:

public slots:
    void setStatus(int status);
    int getStatus();
    void setHeader(const QString &name, const QString &value);
    void setData(const QByteArray &data);
    QString toString();
    QByteArray getData();
};

}

#endif // TCPRESPONSE_H
