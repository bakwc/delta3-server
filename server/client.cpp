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

void Client::sendPing() const
{
    this->send(QString("p:"));
}

void Client::onDataReceived()
{
    QString data = QString::fromLocal8Bit(socket_->readAll());
    qDebug() << "onDataReceived():";
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

void Client::parseData(const QString& data)
{
    this->setSeen();
    parseClientAuth(data);
    parseAdminAuth(data);
    parseList(data);
    parseTransmit(data);
}

bool Client::parseClientAuth(const QString &data)
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

    this->getServer()->resendListToAdmins();

    return true;
}

bool Client::parseAdminAuth(const QString &data)
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

bool Client::parseDisconnect(const QString& data)
{
    if (this->status_==ST_DISCONNECTED)
        return false;

    QRegExp re("d:");
    if (re.indexIn(data)==-1)
        return false;

    this->disconnectFromHost();

    return true;
}


bool Client::parseList(const QString &data)
{
    if (this->status_!=ST_ADMIN)
        return false;

    QRegExp re("l:");

    if (re.indexIn(data)==-1)
        return false;

    qDebug() << "parseList():";

    this->sendList(getServer()->listConnectedClients());

    return true;
}

bool Client::parseTransmit(const QString &data)
{
    if ( !(this->status_==ST_ADMIN ||
           this->status_==ST_CLIENT))
        return false;

    QRegExp re("t:(\\d+):(\\d+):(.*)");
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

quint32 Client::getLastSeen() const
{
    return time(NULL)-lastSeen_;
}

void Client::setSeen()
{
    lastSeen_=time(NULL);
}

void Client::disconnectFromHost()
{
    qDebug() << "disconnectFromHost()";
    socket_->disconnectFromHost();
    this->status_=ST_DISCONNECTED;
    this->getServer()->resendListToAdmins();
}

void Client::sendList(const QString& list)
{
    QString response=QString("l:%1:").arg(list);
    this->send(response);
}
