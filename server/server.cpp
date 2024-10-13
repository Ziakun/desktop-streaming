#include "server.h"

#include <QtCore>
#include <QDataStream>
#include <QDebug>

Server::Server()
{
    init();
}

void Server::init()
{
    if(!server.listen())
    {
        qDebug() << "Unable to start the server";
        return;
    }

    QString ipAddress;
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // use the first non-localhost IPv4 address
    for(const QHostAddress &entry: ipAddressesList)
    {
        if (entry != QHostAddress::LocalHost && entry.toIPv4Address())
        {
            ipAddress = entry.toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    QString msg(QString("The server is running on IP: %1 port: %2").arg(ipAddress).arg(server.serverPort()));

    qDebug() << msg;

    if(!(readyToStream = encoder.init()))
        qDebug() << "Unable to init encoder";

    connect(&server, &QTcpServer::newConnection, this, [this]
            {
                clientConnection = server.nextPendingConnection();

                connect(&timer, &QTimer::timeout, this, QOverload<>::of(&Server::send));
                timer.start(200);
            });
}

void Server::send()
{
    if(!readyToStream)
        return;

    QByteArray block(encoder.encode());

    if(block.isEmpty())
        return;

    QDataStream dataStream(&block, QIODevice::WriteOnly);

    dataStream.setVersion(QDataStream::Qt_6_5);

    {
        QByteArray sizeBlock;
        QDataStream dataStream(&sizeBlock, QIODevice::WriteOnly);

        dataStream << block.size();
        clientConnection->write(sizeBlock);
    }

    dataStream << block;

    if(clientConnection)
        clientConnection->write(block);
}
