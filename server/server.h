#ifndef SERVER_H
#define SERVER_H

#include "encoder.h"

#include <QObject>
#include <QtNetwork>
#include <QTimer>

class Server: public QObject
{
public:
    Server();

public:
    void send();

private:
    void init();

private:
    QTcpServer server;
    Encoder encoder;
    QTimer timer;
    QTcpSocket* clientConnection = nullptr;
    bool readyToStream = false;
};

#endif // SERVER_H
