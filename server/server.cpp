#include "server.h"

Server::Server(QObject *parent):
    QObject(parent),
    tcpServer_(new QTcpServer(this))
{
    connect(tcpServer_,SIGNAL(newConnection()),
            this,SLOT(onNewConnection()));
}

Server::~Server()
{
}

bool Server::start()
{
    startTimer( DEFAULT_TIMER_INTERVAL );
    return tcpServer_->listen(QHostAddress("0.0.0.0"),1235);
}

void Server::onNewConnection()
{
    qDebug() << "Server::onNewConnection(): new anonymous user connected";
    Client *client=new Client(
                tcpServer_->nextPendingConnection(),
                this);
    clients_.insert(client->getId(),client);
}

QByteArray Server::listConnectedClients()
{
    QByteArray result;
    for (auto i=clients_.begin();i!=clients_.end();i++)
        if (i.value()->getStatus()==ST_CLIENT)
        {
            result.append( toBytes(i.key()) );
            result.append( i.value()->getIdHash() );
        }
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

        if (i.value()->getLastSeen()>15)
        {
            i.value()->disconnectFromHost();
            continue;
        }

        if (i.value()->getLastSeen()>5)
        {
            i.value()->sendPing();
        }
    }
}

void Server::resendListToAdmins()
{
    qDebug() << "Sending list!";
    QByteArray clientList=listConnectedClients();
    for (auto i=clients_.begin();i!=clients_.end();i++)
        if (i.value()->getStatus()==ST_ADMIN)
            i.value()->sendList(clientList);
}
