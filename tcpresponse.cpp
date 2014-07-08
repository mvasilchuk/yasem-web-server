#include "tcpresponse.h"

#include <QStringList>

using namespace yasem;

TcpResponse::TcpResponse(QObject *parent) :
    QObject(parent)
{
}

void TcpResponse::setStatus(int status)
{
    this->status = status;
}

int TcpResponse::getStatus()
{
    return this->status;
}

void TcpResponse::setHeader(const QString &name, const QString &value)
{
    this->headers.insert(name, value);
}

void TcpResponse::setData(const QByteArray &data)
{
    this->data = data;
}

QString TcpResponse::toString()
{
    QStringList headerList;
    for(QString name: headers)
    {
        headerList.append(QString("%1:%2\r\n").arg(name).arg(headers.value(name)));
    }
    return QString("HTTP/1.0 %1 %2\r\n%3\r\n%4").arg(status).arg("Ok").arg(headerList.join("")).arg(QString(data));
}

QByteArray TcpResponse::getData()
{
    QStringList headerList;
    for(QString name: headers)
    {
         headerList.append(QString("%1:%2\r\n").arg(name).arg(headers.value(name)));
    }

    QString head = QString("HTTP/1.0 %1 %2\r\n%3\r\n").arg(status).arg("Ok").arg(headerList.join(""));

    QByteArray arr;
    arr.append(head.toLocal8Bit());
    arr.append(data);
    return arr;
}
