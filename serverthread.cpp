#include "serverthread.h"
#include "tcpresponse.h"
#include "pluginmanager.h"
#include "browserplugin.h"
#include "webserverplugin.h"

#include <QDateTime>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDataStream>

using namespace yasem;

ServerThread::ServerThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    webServer = dynamic_cast<WebServerPlugin*>(this->parent()->parent());
    Q_ASSERT(webServer);
    browser = NULL;
}

void ServerThread::run()
{
    // thread starts here
    //qDebug() << " Thread started";

    socket = new QTcpSocket();

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
    //qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

void ServerThread::readyRead()
{

    // get the information
    //QByteArray data = socket->readAll();

    if(browser == NULL)
    {
        browser = dynamic_cast<BrowserPlugin*>(PluginManager::instance()->getByRole(ROLE_BROWSER));
        Q_ASSERT(browser != NULL);

        webServer->setRootDirectory(browser->browserRootDir());
    }

    QString rootDir = webServer->getRootDirectory();

    QHash<QString, QString> headers;
    QString data;

    if (socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));

        QString method = tokens[0];
        QString path = tokens[1];

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
                headers.insert(header.at(0).trimmed(), value.replace("\r\n", "").trimmed());
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
        QFile* file = new QFile(filename);

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
            //qDebug() << "mime for " << filename << "is" << mime.name();

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
                if (socket->state() == QTcpSocket::UnconnectedState) {
                    delete socket;
                    //qDebug() <<"Connection closed";
                }
            }
        }

        if(file)
            file->close();
        socket->close();

    }
}

void ServerThread::disconnected()
{
    //qDebug() << socketDescriptor << " Disconnected";

    socket->deleteLater();
    exit(0);
}
