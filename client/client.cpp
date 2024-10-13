#include "client.h"

Client::Client()
{
    init();
}

void Client::init()
{
    dataStream.setDevice(&socket);
    dataStream.setVersion(QDataStream::Qt_6_5);

    decoder.init();

    connect(&socket, &QIODevice::readyRead, this, &Client::readStreaming);
    connect(&socket, &QAbstractSocket::errorOccurred, this, &Client::displayError);
    connect(&decoder, &Decoder::sendFrame, this, &Client::sendFrame);
}

void Client::startStreaming(const QString &host, int port)
{
    socket.abort();
    socket.connectToHost(host, port);

    qDebug() << "Start streaming. host:" << host << "port:" << port;
}

void Client::readStreaming()
{
    if(dataStream.device()->size() == sizeof(qsizetype))
    {
        dataStream >> blockSize;
        return;
    }

    QByteArray data(dataStream.device()->readAll());

    block.append(data);

    const auto blockSizeDiff = block.size() - blockSize;

    if(blockSizeDiff < 0 || blockSizeDiff > 12)
        return;

    decoder.decode(block);

    blockSize = 0;
    block.clear();
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "The connection was refused by the peer. Make sure the server is running \
            and check that the host name and port settings are correct.";
            break;
    default:
        qDebug() << "The following error occurred:" << socket.errorString();
    }
}
