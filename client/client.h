#ifndef CLIENT_H
#define CLIENT_H

#include "decoder.h"

#include <QtNetwork>

class Client : public QObject
{
    Q_OBJECT
public:
    Client();

public:
    void init();
    void readStreaming();

signals:
    void sendFrame(const QPixmap &pixmap);

public slots:
    void startStreaming(const QString &host, int port);
    void displayError(QAbstractSocket::SocketError socketError);

public:
    QTcpSocket socket;
    QDataStream dataStream;
    QDataStream transaction;
    Decoder decoder;
    QByteArray block;
    qsizetype blockSize = 0;
};

#endif // CLIENT_H
