#include "client.h"

#include "netextract.h"
#include "defines.h"
#include "server.h"
#include "utils.h"
#include "clientinfostorage.h"
#include "logger.h"


namespace delta3
{
    const Client::CommandTableType Client::CommandTable[] = {
        { CMD1_ADM, &Client::parseAdminAuth },
        { CMD1_AUTH, &Client::parseClientAuth },
        { CMD1_LIST, &Client::parseList },
        { CMD1_TRANSMIT, &Client::parseTransmit },
        { CMD1_PING, &Client::sendPong },
        { CMD1_DISCONNECT, &Client::parseDisconnect },
        { CMD1_SETINFO, &Client::parseSetInfo },
    };

    Client::Client(QTcpSocket *socket, ClientInfoStorage *storage, QObject *parent):
        QObject(parent),
        socket_(socket),
        storage_(storage),
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
        //qDebug() << "onDataReceived():";
        buf_+=socket_->readAll();
        setSeen();

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
        for (quint32 i = 0; i < sizeof(CommandTable) / sizeof(*CommandTable); ++i)
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

    QString Client::getOs() const
    {
        return getClientInfo()->os;
    }

    QString Client::getDevice() const
    {
        return getClientInfo()->deviceType;
    }

    QString Client::getCaption() const
    {
        return getClientInfo()->caption;
    }

    ClientStatus Client::getStatus() const
    {
        return status_;
    }

    void Client::parseClientAuth()
    {
        qDebug() << "parseClientAuth():";
        if (this->status_ != ST_CONNECTED)
        {
            qDebug() << "cmd not allowed";
            this->disconnectFromHost();
            return;
        }

        qDebug() << buf_.size();

        if (buf_.size() < CMD1_AUTH_SIZE)
            return;     // not all data avaliable

        this->status_ = ST_CLIENT;

        ClientInfo* clientInfo = new ClientInfo;
        clientInfo->hash = getClientHash(buf_);
        clientInfo->os = getClientOs(buf_);
        clientInfo->deviceType = getClientDevice(buf_);

        ClientInfoStorage::ClientInfo storeInfo;
        storeInfo.hash = clientInfo->hash;
        storeInfo.ip = QHostAddress(getIp());         // TODO: some refactor here!!
        storeInfo.os = clientInfo->os;                // storeInfo for saving client;
        storeInfo.device = clientInfo->deviceType;    // clientInfo - for client? mb not needed?

        storage_->updateClient(storeInfo);  // to storage

        clientInfo->caption=storage_->getCaption(clientInfo->hash);

        this->clientInfo_.reset(clientInfo); // to self

        qDebug() << "new client authorized";

        this->getServer()->resendListToAdmins();

        buf_ = buf_.right(buf_.size() - CMD1_AUTH_SIZE);
        if (buf_.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::parseAdminAuth()
    {
        qDebug() << "parseAmdinAuth():";
        if (this->status_ != ST_CONNECTED)
        {
            qDebug() << "cmd not allowed";
            this->disconnectFromHost();
            return;
        }

        if (buf_.size() < CMD1_ADM_SIZE)
            return;     // not all data avaliable

        if (getAdminLogin(buf_) != "admin" ||
                getAdminPassword(buf_) != "admin")
        {
            qDebug() << "auth failed";
            getServer()->logger.message()
                       << Logger::tr("Failed admin authentication from client ")
                       << Logger::ipToStr(getIp())
                       << Logger::tr(", username ")
                       << getAdminLogin(buf_);
            getServer()->logger.write();

            this->disconnectFromHost();
            return;
        }

        this->status_ = ST_ADMIN;

        AdminInfo* adminInfo = new AdminInfo;
        adminInfo->login = getAdminLogin(buf_);
        adminInfo->pass = getAdminPassword(buf_);
        this->clientInfo_.reset(adminInfo);

        getServer()->logger.message()
                   << Logger::tr("Successful admin authentication from client ")
                   << Logger::ipToStr(getIp())
                   << Logger::tr(", username ")
                   << getAdminLogin(buf_);
        getServer()->logger.write();

        qDebug() << "New admin authorized:";

        buf_ = buf_.right(buf_.size() - CMD1_ADM_SIZE);
        if (buf_.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::parseDisconnect()
    {
        if (buf_.size() < 3) // TODO: remove magic number
            return;     // not all data avaliable

        qDebug() << "parseDisconnect():";
        this->disconnectFromHost();

        buf_=buf_.right(buf_.size() - 3);
        if (buf_.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::sendPong()
    {
        //qDebug() << "Ping received!";
        if (buf_.size()<3) // TODO: remove magic number
            return;     // not all data avaliable

        //qDebug() << "Ping parsed!";

        buf_ = buf_.right(buf_.size() - 3);
        if (buf_.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::parseSetInfo()
    {
        qDebug() << "SetInfo received!";

        if (this->status_!=ST_ADMIN)
        {
            qDebug() << "cmd not allowed";
            this->disconnectFromHost();
            return;
        }

        if (buf_.size()< CMD1_SETINFO_SIZE) // TODO: remove magic number
            return;     // not all data avaliable

        qint16 clientId=getClientId(buf_);
        QString caption=getClientCaption(buf_);

        getServer()->setClientCaption(clientId,caption);

        buf_ = buf_.right(buf_.size() - CMD1_SETINFO_SIZE);
        if (buf_.size() > 0)
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
        if ( !(this->status_ == ST_ADMIN ||
               this->status_ == ST_CLIENT))
        {
            qDebug() << "cmd not allowed";
            this->disconnectFromHost();
            return;
        }


        if (buf_.size()<9) // TODO: remove magic number
            return;     // not all data avaliable

        qDebug() << "buf size" << buf_.size();
        qDebug() << "packet len" << getPacketLength(buf_);

        if (buf_.size()<getPacketLength(buf_)+9) // TODO: remove magic number
            return; // not all data avaliable

        qint16 clientId=getClientId(buf_);

        QByteArray cmd=getPacketData(buf_);

        if (this->status_ == ST_CLIENT)
            if (getClientInfo()->admins.find(clientId) ==
                    getClientInfo()->admins.end())
            {   // If no admin speaking with client
                qDebug() << "cmd not allowed";
                this->disconnectFromHost();
                return;
            }

        if (this->status_ == ST_ADMIN)
        {
            this->getServer()->setAdminTalkingWithClient(clientId, this->getId());
        }

        QByteArray response;
        response.append(CSPYP1_PROTOCOL_ID);
        response.append(CSPYP1_PROTOCOL_VERSION);
        response.append(CMD1_TRANSMIT);
        response.append(toBytes(getId()));
        response.append(toBytes(cmd.size()));
        response.append(cmd);

        auto destClient = getServer()->searchClient(clientId);
        qDebug() << "Client to ID:" << clientId;

        if (destClient != getServer()->clientEnd())
        {
            qDebug() << "transmiting data..";
            destClient.value()->send(response);
        }

        buf_=buf_.right(buf_.size() - (getPacketLength(buf_) + 9));

        if (buf_.size() > 0)
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
        return time(NULL) - lastSeen_;
    }

    void Client::addTalkingWithAdmin(qint16 adminId)
    {
        if (this->status_ != ST_CLIENT)
            return;
        getClientInfo()->admins.insert(adminId);
    }

    qint32 Client::getIp() const
    {
        return socket_->peerAddress().toIPv4Address();
    }

    void Client::setSeen()
    {
        lastSeen_ = time(NULL);
    }

    void Client::setCaption(const QString& caption)
    {
        if (status_!=ST_CLIENT)
            return;
        this->getClientInfo()->caption=caption;
    }

    void Client::disconnectFromHost()
    {
        qDebug() << "disconnectFromHost()";
        socket_->disconnectFromHost();

        clientInfo_.reset();

        status_ = ST_DISCONNECTED;
        getServer()->resendListToAdmins();
    }

    void Client::sendList(const QByteArray& list)
    {
        QByteArray cmd;
        cmd.append(CSPYP1_PROTOCOL_ID);
        cmd.append(CSPYP1_PROTOCOL_VERSION);
        cmd.append(CMD1_RLIST);
        cmd.append(list);
        this->send(cmd);
    }
}
