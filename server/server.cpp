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

QString Server::listConnectedClients()
{
    QString result;

    for (auto i=clients_.begin();i!=clients_.end();i++)
        if (i.value()->getStatus()==ST_CLIENT)
            result+=QString("%1;%2;")
                    .arg(i.key())
                    .arg(i.value()->getIdHash());
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
