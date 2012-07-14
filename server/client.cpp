#include "client.h"

#include "defines.h"
#include "server.h"
#include "utils.h"

namespace
{
    using namespace delta3;

    qint8 getProtoId(const QByteArray& buffer)
    {
        return (qint8)(buffer[0]);
    }

    qint8 getProtoVersion(const QByteArray& buffer)
    {
        return (qint8)(buffer[1]);
    }

    Cspyp1Command getCommand(const QByteArray& buffer)
    {
        return (Cspyp1Command)(buffer[2]);
    }

    QByteArray getClientHash(const QByteArray& buffer)
    {
        return buffer.mid(3,16);
    }

    QString getAdminLogin(const QByteArray& buffer)
    {
        return buffer.mid(3,22);
    }

    QString getAdminPassword(const QByteArray& buffer)
    {
        return buffer.mid(25,22);
    }

    qint32 getPacketLength(const QByteArray& buffer)
    {
        return fromBytes<qint32>(buffer.mid(5,4));
    }

    qint16 getClientId(const QByteArray& buffer)
    {
        return fromBytes<qint16>(buffer.mid(3,2));
    }

    QByteArray getPacketData(const QByteArray& buffer)
    {
        return buffer.mid(9,getPacketLength(buffer));
    }
}

namespace delta3
{
    const Client::CommandTableType Client::CommandTable[] = {
        { CMD1_ADM, &Client::parseAdminAuth },
        { CMD1_AUTH, &Client::parseClientAuth },
        { CMD1_LIST, &Client::parseList },
        { CMD1_TRANSMIT, &Client::parseTransmit },
        { CMD1_PING, &Client::parsePing },
        { CMD1_DISCONNECT, &Client::parseDisconnect },
    };

    Client::Client(QTcpSocket *socket, QObject *parent):
        QObject(parent),
        socket_(socket),
        clientInfo_(),
        status_(ST_CONNECTED)
    {
        connect(socket_,SIGNAL(readyRead()),
                this,SLOT(onDataReceived()));
    }

    void Client::send(const QByteArray &cmd) const
    {
        socket_->write(cmd);
    }

    void Client::ping() const
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

        if (getProtoId(buf_)!=CSPYP1_PROTOCOL_ID ||
                getProtoVersion(buf_)!=CSPYP1_PROTOCOL_VERSION)
        {
            // wrong packet - disconnecting client
            qDebug() << "PROTOCOL ERROR!";
            disconnectFromHost();
            return;
        }


        const Cspyp1Command command = getCommand(buf_);
        for (int i = 0; i < sizeof(CommandTable) / sizeof(*CommandTable); ++i)
        {
            if (CommandTable[i].command == command)
            {
                (this->*CommandTable[i].function)();
                return;
            }
        }

        qDebug() << "PROTOCOL ERROR!";
        disconnectFromHost();
    }

    qint16 Client::getId() const
    {
        return socket_->socketDescriptor();
    }

    QByteArray Client::getIdHash() const
    {
        return getClientInfo()->hash;
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
        clientInfo->hash = getClientHash(buf_);
        this->clientInfo_.reset(clientInfo);

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

        if (getAdminLogin(buf_)!="admin" ||
                getAdminPassword(buf_)!="admin")
        {
            qDebug() << "auth failed";
            this->disconnectFromHost();
            return;
        }

        this->status_ = ST_ADMIN;

        AdminInfo* adminInfo=new AdminInfo;
        adminInfo->login = getAdminLogin(buf_);
        adminInfo->pass = getAdminPassword(buf_);
        this->clientInfo_.reset(adminInfo);

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

        if (buf_.size()<getPacketLength(buf_)+9) // TODO: remove magic number
            return; // not all data avaliable

        qint16 clientId=getClientId(buf_);

        QByteArray cmd=getPacketData(buf_);

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

        buf_=buf_.right(buf_.size()-(getPacketLength(buf_)+9));

        if (buf_.size()>0)
            onDataReceived();   // If something in buffer - parse again
    }

    Server* Client::getServer() const
    {
        return static_cast<Server*>(parent());
    }

    Client::ClientInfo* Client::getClientInfo() const
    {
        return static_cast<ClientInfo*>(clientInfo_.get());
    }

    Client::AdminInfo* Client::getAdminInfo() const
    {
        return static_cast<AdminInfo*>(clientInfo_.get());
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

        clientInfo_.reset();

        status_=ST_DISCONNECTED;
        getServer()->resendListToAdmins();
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
}
