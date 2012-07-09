#include "client.h"

Client::Client(QTcpSocket *socket):
    QObject(0),
    socket_(socket)
{
    connect(socket_,SIGNAL(readyRead()),
            this,SLOT(onDataReceived()));
    status_=ST_CONNECTED;
}

void Client::send(const QByteArray &cmd)
{

}

void Client::onDataReceived()
{

}

qint32 Client::getId()
{
    return socket_->socketDescriptor();
}
