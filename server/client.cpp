#include "client.h"
#include "server.h"

Client::Client(QTcpSocket *socket, QObject *parent):
    QObject(parent),
    socket_(socket)
{
    connect(socket_,SIGNAL(readyRead()),
            this,SLOT(onDataReceived()));
    status_=ST_CONNECTED;
}

void Client::send(const QByteArray &cmd) const
{
    socket_->write(cmd);
}

void Client::send(const QString &cmd) const
{
    socket_->write(cmd.toLocal8Bit());
}

void Client::onDataReceived()
{
    QByteArray data=socket_->readAll();
    qDebug() << "onDataReceived():" << data;
    parseData(data);
}

qint32 Client::getId() const
{
    return socket_->socketDescriptor();
}

QString Client::getIdHash() const
{
    return clientIdHash_;
}

ClientStatus Client::getStatus() const
{
    return status_;
}

void Client::parseData(const QByteArray& data)
{
    parseClientAuth(data);
    parseAdminAuth(data);
    parseList(data);
    parseTransmit(data);
}

bool Client::parseClientAuth(const QByteArray& data)
{
    if (this->status_!=ST_CONNECTED)
        return false;

    QRegExp re("cspycli:(\\d+):(\\w+):");

    if (re.indexIn(data)==-1)
        return false;

    if (re.cap(1).toInt()!=1 ||
            re.cap(2).length()!=32)
        return false;

    this->status_ = ST_CLIENT;
    this->clientIdHash_ = re.cap(2);

    qDebug() << "parseClientAuth(): new client:"
             << re.cap(2);

    return true;
}

bool Client::parseAdminAuth(const QByteArray& data)
{
    if (this->status_!=ST_CONNECTED)
        return false;

    QRegExp re("cspyadm:(\\d+):(\\w+):(\\w+):");

    if (re.indexIn(data)==-1)
        return false;

    if (re.cap(1).toInt()!=1 ||
            re.cap(2)!="admin" ||
            re.cap(3)!="admin")
        // TODO: check password
        return false;

    this->status_ = ST_ADMIN;
    this->clientIdHash_ = re.cap(2);

    qDebug() << "parseAdminAuth(): new admin:"
             << re.cap(2);

    return true;
}


bool Client::parseList(const QByteArray& data)
{
    if (this->status_!=ST_ADMIN)
        return false;

    QRegExp re("l:");

    if (re.indexIn(data)==-1)
        return false;

    qDebug() << "parseList():";

    QString response;
    response=getServer()->listConnectedClients();

    response=QString("l:%1:").arg(response);
    this->send(response);

    return true;
}

bool Client::parseTransmit(const QByteArray& data)
{
    if (this->status_!=ST_CONNECTED)
        return false;

    QRegExp re("t:(\\d+):(\\d+):(*)");

    if (re.indexIn(data)==-1)
        return false;

    qint32 packetLen=re.cap(2).toInt();
    qint32 clientId=re.cap(1).toInt();
    QByteArray cmd=re.cap(3).toLocal8Bit();
    cmd=cmd.left(packetLen);

    auto strCmd=QString("f:%1:%2:")
            .arg(getId())
            .arg(cmd.size());

    cmd=strCmd.toLocal8Bit()+cmd+':';

    if (re.cap(3).size()<packetLen)
        return false;

    auto destClient=getServer()->searchClient(clientId);

    if (destClient==getServer()->clientEnd())
        return false;

    qDebug() << "parseTransmit(): transmiting data..";

    destClient.value()->send(cmd);

    return true;
}

Server* Client::getServer()
{
    return static_cast<Server*>(parent());
}
