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
