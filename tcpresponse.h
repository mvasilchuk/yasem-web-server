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
    int m_status;
    QHash<QString, QString> headers;
    QByteArray m_data;

signals:

public slots:
    void setStatus(int m_status);
    int getStatus();
    void setHeader(const QString &name, const QString &value);
    void setData(const QByteArray &m_data);
    QString toString();
    QByteArray getData();
};

}

#endif // TCPRESPONSE_H
