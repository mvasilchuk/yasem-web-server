#include "serverthread.h"
#include "tcpresponse.h"
#include "pluginmanager.h"
#include "browser.h"
#include "webserverplugin.h"
#include "macros.h"

#include <QDateTime>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDataStream>
#include <QNetworkRequest>

using namespace yasem;

ServerThread::ServerThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    webServer = dynamic_cast<SDK::WebServerPlugin*>(this->parent()->parent());
    Q_ASSERT(webServer);
}

ServerThread::~ServerThread()
{
    if(webServer)
        webServer->deleteLater();
}

void ServerThread::run()
{
    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();
    socket->moveToThread(this);

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

void ServerThread::readyRead()
{
    QString rootDir = SDK::Browser::instance()->browserRootDir();

    DEBUG() << "rootDir" << rootDir;

    QHash<QByteArray, QByteArray> headers;
    QString data;

    if (socket->canReadLine()) {
        QString first_line = socket->readLine();
        //QStringList tokens = first_line.split(QRegExp("[ \r\n][ \r\n]*"));
        QStringList tokens = first_line.split(" ");

        QString method = tokens[0];
        QString path = tokens[1];

        DEBUG() << "first_line" << first_line;

        bool parseHeaders = true;
        while(socket->canReadLine())
        {
            QString line = socket->readLine();
            if(line == "\r\n")
            {
                parseHeaders = false;
            }

            if(parseHeaders)
            {
                QStringList header = line.split(":");
                QString value = header.at(1);
                headers.insert(header.at(0).trimmed().toUtf8(), value.replace("\r\n", "").trimmed().toUtf8());
            }
            else
            {
                data = socket->readAll();
            }
        }

        TcpResponse response;
        response.setStatus(200);
        response.setHeader("Content-Type", "text/html; charset=\"utf-8\"");

        QString fullPath = rootDir + path;
        QStringList filePathWithArgs = fullPath.split("?");

        QString filename = filePathWithArgs[0].trimmed();
        QFile* file = new QFile(QUrl(filename).toLocalFile());

        qDebug() << "opening file" << filename;

        bool result = file->open(QFile::ReadOnly);
        if(!result)
        {
            ERROR() << QString("Cannot open file: %1 -> %2").arg(filename).arg(file->errorString());
        }
        else
        {
            //QList<QMimeType> mimes = mimeDatabase.mimeTypesForFileName(filename);
            QMimeType mime = mimeDatabase.mimeTypeForFileNameAndData(filename, file);
            qDebug() << "mime for " << filename << "is" << mime.name();

            QByteArray data = file->readAll();
            response.setHeader("Content-Type", mime.name());
            response.setData(data);

            if (method == "GET") {
                QDataStream os(socket);
                //os.setAutoDetectUnicode(true);
                //os << r;

                QByteArray rawData = response.getData();
                os.writeRawData(rawData, rawData.size());

                qDebug() << "data:" << rawData; 
            }

            socket->close();

            if (socket->state() == QTcpSocket::UnconnectedState) {
                delete socket;
                qDebug() <<"Connection closed";
            }
        }

        if(file)
            file->close();
        socket->close();

    }
}

void ServerThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";

    if(socket != NULL && socket->isOpen())
    {
        socket->close();
        socket->deleteLater();
    }
    DEBUG() << "2";
    socket->thread()->quit();
    this->quit();
}
