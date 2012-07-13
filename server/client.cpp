#include "client.h"
#include "server.h"
#include "utils.hpp"

Client::Client(QTcpSocket *socket, QObject *parent):
    QObject(parent),
    socket_(socket)
{
    connect(socket_,SIGNAL(readyRead()),
            this,SLOT(onDataReceived()));
    status_=ST_CONNECTED;
    clientInfo_=NULL;
}

void Client::send(const QByteArray &cmd) const
{  
    socket_->write(cmd);
}

void Client::sendPing() const
{
    QByteArray cmd;
    cmd.append(CSPYP1_PROTOCOL_ID);
    cmd.append(CSPYP1_PROTOCOL_VERSION);
    cmd.append(CMD1_PING);
    this->send(cmd);
}

void Client::onDataReceived()
{
    qDebug() << "onDataReceived():";
    buf_+=socket_->readAll();

    if (buf_.size()<3) return; // if we don't read header

    if (getProtoId()!=CSPYP1_PROTOCOL_ID ||
            getProtoVerstion()!=CSPYP1_PROTOCOL_VERSION)
    {
        // wrong packet - disconnecting client
        qDebug() << "PROTOCOL ERROR!";
        this->disconnectFromHost();
        return;
    }

    switch (getCommand())
    {
    case CMD1_ADM:
        parseAdminAuth();
        break;
    case CMD1_AUTH:
        parseClientAuth();
        break;
    case CMD1_LIST:
        parseList();
        break;
    case CMD1_TRANSMIT:
        parseTransmit();
        break;
    case CMD1_PING:
        parsePing();
        break;
    case CMD1_DISCONNECT:
        parseDisconnect();
        break;
    default:
        break;
    }

}

qint16 Client::getId() const
{
    return socket_->socketDescriptor();
}

QByteArray Client::getIdHash() const
{
    ClientInfo* info=reinterpret_cast<ClientInfo*>(clientInfo_);
    return info->hash;
}

ClientStatus Client::getStatus() const
{
    return status_;
}

void Client::parseClientAuth()
{
    qDebug() << "parseClientAuth():";
    if (this->status_!=ST_CONNECTED)
    {
        qDebug() << "cmd not allowed";
        this->disconnectFromHost();
        return;
    }

    if (buf_.size()<19) // TODO: remove magic number
        return;     // not all data avaliable

    this->status_ = ST_CLIENT;

    ClientInfo* clientInfo=new ClientInfo;
    clientInfo->hash = getClientHash();
    this->clientInfo_ = reinterpret_cast<void*>(clientInfo);

    qDebug() << "new client authorized";

    this->getServer()->resendListToAdmins();

    buf_=buf_.right(buf_.size()-19);
    if (buf_.size()>0)
        onDataReceived();   // If something in buffer - parse again
}

void Client::parseAdminAuth()
{
    qDebug() << "parseAmdinAuth():";
    if (this->status_!=ST_CONNECTED)
    {
        qDebug() << "cmd not allowed";
        this->disconnectFromHost();
        return;
    }

    if (buf_.size()<47) // TODO: remove magic number
        return;     // not all data avaliable

    if (getAdminLogin()!="admin" ||
            getAdminPassword()!="admin")
    {
        qDebug() << "auth failed";
        this->disconnectFromHost();
        return;
    }

    this->status_ = ST_ADMIN;

    AdminInfo* adminInfo=new AdminInfo;
    adminInfo->login = getAdminLogin();
    adminInfo->pass = getAdminPassword();
    this->clientInfo_ = reinterpret_cast<void*>(adminInfo);

    qDebug() << "New admin authorized:";

    buf_=buf_.right(buf_.size()-47);
    if (buf_.size()>0)
        onDataReceived();   // If something in buffer - parse again
}

void Client::parseDisconnect()
{
    if (buf_.size()<3) // TODO: remove magic number
        return;     // not all data avaliable

    this->disconnectFromHost();

    buf_=buf_.right(buf_.size()-3);
    if (buf_.size()>0)
        onDataReceived();   // If something in buffer - parse again
}

void Client::parsePing()
{
    qDebug() << "Ping received!";
    if (buf_.size()<3) // TODO: remove magic number
        return;     // not all data avaliable

    qDebug() << "Ping parsed!";
    setSeen();

    buf_=buf_.right(buf_.size()-3);
    if (buf_.size()>0)
        onDataReceived();   // If something in buffer - parse again
}

void Client::parseList()
{
    qDebug() << "parseList():";
    if (this->status_!=ST_ADMIN)
    {
        qDebug() << "cmd not allowed";
        this->disconnectFromHost();
        return;
    }

    if (buf_.size()<3) // TODO: remove magic number
        return;     // not all data avaliable

    buf_=buf_.right(buf_.size()-3);
    if (buf_.size()>0)
        onDataReceived();   // If something in buffer - parse again

    this->sendList(getServer()->listConnectedClients());
}

void Client::parseTransmit()
{
    qDebug() << "parseTransmit():";
    if ( !(this->status_==ST_ADMIN ||
           this->status_==ST_CLIENT))
    {
        qDebug() << "cmd not allowed";
        this->disconnectFromHost();
        return;
    }

    if (buf_.size()<9) // TODO: remove magic number
        return;     // not all data avaliable

    if (buf_.size()<getPacketLength()+9) // TODO: remove magic number
        return; // not all data avaliable

    qint16 clientId=getClientId();

    QByteArray cmd=getPacketData();

    QByteArray response;

    response.append(CSPYP1_PROTOCOL_ID);
    response.append(CSPYP1_PROTOCOL_VERSION);
    response.append(CMD1_TRANSMIT);
    response.append(toBytes(getId()));
    response.append(toBytes(cmd.size()));
    response.append(cmd);


    auto destClient=getServer()->searchClient(clientId);

    if (destClient!=getServer()->clientEnd())
    {
        qDebug() << "transmiting data..";
        destClient.value()->send(cmd);
    }

    buf_=buf_.right(buf_.size()-(getPacketLength()+9));

    if (buf_.size()>0)
        onDataReceived();   // If something in buffer - parse again
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

    if (this->status_==ST_ADMIN)
    {
        AdminInfo *adminInfo = reinterpret_cast<AdminInfo*>(this->clientInfo_);
        if (adminInfo!=NULL)
            delete adminInfo;
    } else
    if (this->status_==ST_CLIENT)
    {
        ClientInfo *clientInfo = reinterpret_cast<ClientInfo*>(this->clientInfo_);
        if (clientInfo!=NULL)
            delete clientInfo;
    }

    this->status_=ST_DISCONNECTED;
    this->getServer()->resendListToAdmins();
}

void Client::sendList(const QByteArray& list)
{
    QByteArray cmd;
    cmd.append(CSPYP1_PROTOCOL_ID);
    cmd.append(CSPYP1_PROTOCOL_VERSION);
    cmd.append(CMD1_LIST);
    cmd.append(list);
    this->send(cmd);
}


qint8 Client::getProtoId()
{
    return (qint8)(buf_[0]);
}

qint8 Client::getProtoVerstion()
{
    return (qint8)(buf_[1]);
}

Cspyp1Command Client::getCommand()
{
    return (Cspyp1Command)(char)(buf_[2]);
}

QByteArray Client::getClientHash()
{
    return buf_.mid(3,16);
    /*
    QString myQString;
    QTextStream myStream(&myQString);
    for (auto i=buf_.begin()+3;i<buf_.begin()+19;i++)
        myStream << hex << (qint8)(*i);
    return myQString;*/
}

QString Client::getAdminLogin()
{
    return buf_.mid(3,22);
}

QString Client::getAdminPassword()
{
    return buf_.mid(25,22);
}

qint32 Client::getPacketLength()
{
    return fromBytes<qint32>(buf_.mid(5,4));
}

qint16 Client::getClientId()
{
    return fromBytes<qint16>(buf_.mid(3,2));
}

QByteArray Client::getPacketData()
{
    return buf_.mid(9,getPacketLength());
}
