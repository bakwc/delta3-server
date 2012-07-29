#include "server.h"

#include "client.h"
#include "utils.h"

namespace
{
    const quint16 DEFAULT_PORT = 1235;

    const quint16 MAX_UNACTIVE_TIME = 15;
    const quint16 TIME_TO_PING = 5;
}

namespace delta3
{
    Server::Server(QObject *parent):
        QObject(parent),
        tcpServer_(new QTcpServer(this)),
        storage_(new ClientInfoStorage(this))
    {
        connect(tcpServer_,SIGNAL(newConnection()),
                this,SLOT(onNewConnection()));
        storage_->load();
        logger.openLogFile("delta3-server.log");
        logger.setDefaultStream(Logger::FILE);
        logger.message() << Logger::toChar(tr("Delta3 Server started"));
        logger.write();

    }

    Server::~Server()
    {
    }

    bool Server::start()
    {
        startTimer( DEFAULT_TIMER_INTERVAL );
        return tcpServer_->listen(QHostAddress("0.0.0.0"), DEFAULT_PORT);
    }

    void Server::onNewConnection()
    {
        qDebug() << "Server::onNewConnection(): new anonymous user connected";

        Client *client=new Client(
                tcpServer_->nextPendingConnection(),
                storage_, this);
        clients_.insert(client->getId(),client);
        logger.message()
                << Logger::toChar(tr("New connection from "))
                << Logger::ipToStr(client->getIp());
        logger.write();
    }

    QByteArray Server::listConnectedClients()
    {
        QByteArray result;
        qint16 clientNum=0;
        for (auto i=clients_.begin();i!=clients_.end();i++)
        {
            if (i.value()->getStatus()==ST_CLIENT)
            {
                QByteArray clientInfo;
                clientInfo.append( toBytes((qint16)i.key()) );
                clientInfo.append( i.value()->getIdHash() );
                clientInfo.append( toBytes(i.value()->getOs(), 20 ), 20 );
                clientInfo.append( toBytes(i.value()->getDevice(), 20 ), 20 );
                clientInfo.append( toBytes(i.value()->getIp()), 4);
                clientInfo.append( toBytes(i.value()->getCaption(), 30 ), 30 );
                result.append(clientInfo);
                clientNum++;
            }
        }
        qDebug() << "Connected clients: " << clientNum;
        result=toBytes(clientNum)+result;
        return result;
    }

    Clients::iterator Server::searchClient(qint32 clientId)
    {
        return clients_.find(clientId);
    }

    Clients::iterator Server::clientEnd()
    {
        return clients_.end();
    }

    void Server::timerEvent( QTimerEvent* event )
    {
        Q_UNUSED( event );

        for (auto i=clients_.begin();i!=clients_.end();i++)
        {
            if (i.value()->getStatus()==ST_DISCONNECTED)
                continue;
                // TODO: remove disconnected clients from clients_ to prevent memory leak

            if (i.value()->getLastSeen()>MAX_UNACTIVE_TIME)
            {
                qDebug() << "Client inactive!";
                logger.message()
                        << tr("Disconnecting inactive client: ")
                        << QHostAddress(i.value()->getIp()).toString().toLocal8Bit().data();
                logger.write();
                i.value()->disconnectFromHost();
                continue;
            }

            if (i.value()->getLastSeen()>TIME_TO_PING)
            {
                i.value()->ping();
            }
        }
        storage_->save();
    }

    void Server::resendListToAdmins()
    {
        qDebug() << "Sending list!";
        QByteArray clientList=listConnectedClients();
        for (auto i=clients_.begin();i!=clients_.end();i++)
            if (i.value()->getStatus()==ST_ADMIN)
                i.value()->sendList(clientList);
    }

    void Server::setClientCaption(qint16 clientId, const QString& caption)
    {
        auto i=clients_.find(clientId);
        if (i==clients_.end())
            return;
        i.value()->setCaption(caption);
        storage_->setCaption(i.value()->getIdHash(),caption);
        i.value()->getIdHash();
    }

    void Server::setAdminTalkingWithClient(qint16 clientId, qint16 adminId)
    {
        auto i=clients_.find(clientId);
        if (i==clients_.end())
            return;
        i.value()->addTalkingWithAdmin(adminId);
    }
}
