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
        _socket(socket),
        _storage(storage),
        _clientInfo(),
        _status(ST_CONNECTED)
    {
        connect(_socket,SIGNAL(readyRead()),
                this,SLOT(onDataReceived()));
    }

    void Client::send(const QByteArray &cmd) const
    {
        _socket->write(cmd);
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
        _buf+=_socket->readAll();
        setSeen();

        if (_buf.size()<3) return; // if we don't read header

        if (getProtoId(_buf)!=CSPYP1_PROTOCOL_ID ||
                getProtoVersion(_buf)!=CSPYP1_PROTOCOL_VERSION)
        {
            // wrong packet - disconnecting client
            qDebug() << "PROTOCOL ERROR!";
            disconnectFromHost();
            return;
        }


        const Cspyp1Command command = getCommand(_buf);
        for (quint32 i=0; i<sizeof(CommandTable)/sizeof(*CommandTable); ++i)
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
        return _socket->socketDescriptor();
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
        return _status;
    }

    void Client::parseClientAuth()
    {
        //qDebug() << "parseClientAuth():";
        if (this->_status != ST_CONNECTED)
            this->disconnectWithError(tr("command not allowed"));

        //qDebug() << _buf.size();

        if (_buf.size() < CMD1_AUTH_SIZE)
            return;     // not all data avaliable

        this->_status = ST_CLIENT;

        ClientInfo* clientInfo = new ClientInfo;
        clientInfo->hash = getClientHash(_buf);
        clientInfo->os = getClientOs(_buf);
        clientInfo->deviceType = getClientDevice(_buf);

        ClientInfoStorage::ClientInfo storeInfo;
        storeInfo.hash = clientInfo->hash;
        storeInfo.ip = QHostAddress(getIp());         // TODO: some refactor here!!
        storeInfo.os = clientInfo->os;                // storeInfo for saving client;
        storeInfo.device = clientInfo->deviceType;    // clientInfo - for client? mb not needed?

        _storage->updateClient(storeInfo);  // to storage

        clientInfo->caption=_storage->getCaption(clientInfo->hash);

        this->_clientInfo.reset(clientInfo); // to self

        qDebug() << tr("Successful client authentication from ip address: ")
                    << Logger::hostAddressToStr(storeInfo.ip);
        getLogger().message()
                   << tr("Successful client authentication from ip address: ")
                   << Logger::hostAddressToStr(storeInfo.ip);
        getLogger().write();

        this->getServer()->resendListToAdmins();

        _buf = _buf.right(_buf.size() - CMD1_AUTH_SIZE);
        if (_buf.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::parseAdminAuth()
    {
        //qDebug() << "parseAmdinAuth():";
        if (this->_status != ST_CONNECTED)
          this->disconnectWithError(tr("command not allowed"));

        if (_buf.size() < CMD1_ADM_SIZE)
            return;     // not all data avaliable

        if (getAdminLogin(_buf) != "admin" ||
                getAdminPassword(_buf) != "admin")
        {
            qDebug() << "auth failed";
            getLogger().message()
                       << tr("Failed admin authentication from client ")
                       << Logger::ipToStr(getIp())
                       << tr(", username ")
                       << getAdminLogin(_buf);
            getLogger().write();

            this->disconnectFromHost();
            return;
        }

        this->_status = ST_ADMIN;

        AdminInfo* adminInfo = new AdminInfo;
        adminInfo->login = getAdminLogin(_buf);
        adminInfo->pass = getAdminPassword(_buf);
        this->_clientInfo.reset(adminInfo);

        getLogger().message()
                   << tr("Successful admin authentication from client ")
                   << Logger::ipToStr(getIp())
                   << tr(", username ")
                   << adminInfo->login;
        getLogger().write();

        qDebug() << tr("Successful admin authentication from client ")
                 << Logger::ipToStr(getIp())
                 << tr(", username ")
                 << adminInfo->login;

        _buf = _buf.right(_buf.size() - CMD1_ADM_SIZE);
        if (_buf.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::parseDisconnect()
    {
        if (_buf.size() < 3) // TODO: remove magic number
            return;     // not all data avaliable

        getLogger().message() << QHostAddress(getIp()).toString()
                              << tr(" disconnected");

        qDebug() << QHostAddress(getIp()).toString()
                 << tr(" disconnected");

        this->disconnectFromHost();

        _buf=_buf.right(_buf.size() - 3);
        if (_buf.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::sendPong()
    {
        //qDebug() << "Ping received!";
        if (_buf.size()<3) // TODO: remove magic number
            return;     // not all data avaliable

        //qDebug() << "Ping parsed!";

        _buf = _buf.right(_buf.size() - 3);
        if (_buf.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    void Client::parseSetInfo()
    {
        //qDebug() << "SetInfo received!";

        if (this->_status!=ST_ADMIN)
            this->disconnectWithError(tr("command not allowed"));

        if (_buf.size()< CMD1_SETINFO_SIZE)
            return;     // not all data avaliable

        qint16 clientId=getClientId(_buf);
        QString caption=getClientCaption(_buf);

        getServer()->setClientCaption(clientId,caption);

        _buf = _buf.right(_buf.size() - CMD1_SETINFO_SIZE);
        if (_buf.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }


    void Client::parseList()
    {
        //qDebug() << "parseList():";
        if (this->_status!=ST_ADMIN)
            this->disconnectWithError(tr("command not allowed"));

        if (_buf.size()<3) // TODO: remove magic number
            return;     // not all data avaliable

        _buf=_buf.right(_buf.size()-3);
        if (_buf.size()>0)
            onDataReceived();   // If something in buffer - parse again

        this->sendList(getServer()->listConnectedClients());
    }

    void Client::parseTransmit()
    {
        //qDebug() << "parseTransmit():";
        if ( !(this->_status == ST_ADMIN ||
               this->_status == ST_CLIENT))
            this->disconnectWithError(tr("command not allowed"));

        if (_buf.size()>=MAX_PACKET_LENGTH)
            this->disconnectWithError(tr("too long packet"));

        if (_buf.size()<9) // TODO: remove magic number
            return;     // not all data avaliable

        //qDebug() << "buf size" << _buf.size();
        //qDebug() << "packet len" << getPacketLength(_buf);

        if (_buf.size()<getPacketLength(_buf)+9) // TODO: remove magic number
            return; // not all data avaliable

        qint16 clientId=getClientId(_buf);

        QByteArray cmd=getPacketData(_buf);

        if (this->_status == ST_CLIENT)
            if (getClientInfo()->admins.find(clientId) ==
                    getClientInfo()->admins.end())
               // If no admin speaking with client
                this->disconnectWithError(tr("command not allowed"));

        if (this->_status == ST_ADMIN)
        {
            this->getServer()->setAdminTalkingWithClient(clientId, this->getId());
            /*  TODO: Fix this code to work!
            auto cli=getServer()->searchClient(clientId);
            QString cliIp;
            if (cli!=getServer()->clientEnd())
                cliIp=QHostAddress(cli.value()->getIp()).toString();
            getLogger().message()
                       << tr("Admin ") << this->getAdminInfo()->login
                       << tr("started session with client ")
                       << cliIp;
            getLogger().write();
            qDebug() << tr("Admin ") << this->getAdminInfo()->login
                     << tr("started session with client ")
                     << cliIp;
                     */
        }

        QByteArray response;
        response.append(CSPYP1_PROTOCOL_ID);
        response.append(CSPYP1_PROTOCOL_VERSION);
        response.append(CMD1_TRANSMIT);
        response.append(toBytes(getId()));
        response.append(toBytes(cmd.size()));
        response.append(cmd);

        auto destClient = getServer()->searchClient(clientId);
        //qDebug() << "Client to ID:" << clientId;

        if (destClient != getServer()->clientEnd())
        {
            //qDebug() << "transmiting data..";
            destClient.value()->send(response);
        }

        _buf=_buf.right(_buf.size() - (getPacketLength(_buf) + 9));

        if (_buf.size() > 0)
            onDataReceived();   // If something in buffer - parse again
    }

    Server* Client::getServer() const
    {
        return static_cast<Server*>(parent());
    }

    Logger& Client::getLogger() const
    {
        return getServer()->getLogger();
    }

    Client::ClientInfo* Client::getClientInfo() const
    {
        return static_cast<ClientInfo*>(_clientInfo.get());
    }

    Client::AdminInfo* Client::getAdminInfo() const
    {
        return static_cast<AdminInfo*>(_clientInfo.get());
    }

    quint32 Client::getLastSeen() const
    {
        return time(NULL) - _lastSeen;
    }

    void Client::addTalkingWithAdmin(qint16 adminId)
    {
        if (this->_status != ST_CLIENT)
            return;
        getClientInfo()->admins.insert(adminId);
    }

    qint32 Client::getIp() const
    {
        return _socket->peerAddress().toIPv4Address();
    }

    void Client::setSeen()
    {
        _lastSeen = time(NULL);
    }

    void Client::setCaption(const QString& caption)
    {
        if (_status!=ST_CLIENT)
            return;
        this->getClientInfo()->caption=caption;
    }

    void Client::disconnectFromHost()
    {
        //qDebug() << "disconnectFromHost()";
        _socket->disconnectFromHost();

        _clientInfo.reset();

        _status = ST_DISCONNECTED;
        getServer()->resendListToAdmins();
    }

    void Client::disconnectWithError(const QString& error)
    {
        getLogger().message() << QHostAddress(getIp()).toString()
                              << " - error (disoconnecting) - "
                              << error;
        qDebug() << QHostAddress(getIp()).toString()
                 << " - error (disoconnecting) - "
                 << error;
        getLogger().write();
        this->disconnectFromHost();
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
