#pragma once

#include <QObject>
#include <QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QMap>
#include <QTimerEvent>
#include "client.h"
#include "defines.h"

typedef QMap<qint32,Client*> Clients;

class Server: public QObject
{
    Q_OBJECT
public:
    Server(QObject* parent = 0);
    ~Server();
    bool start();
    QString listConnectedClients();
    Clients::iterator searchClient(qint32 clientId);
    Clients::iterator clientEnd();
    void resendListToAdmins();
private slots:
    void onNewConnection();
private:
    void timerEvent( QTimerEvent* event );
private:
    QTcpServer* tcpServer_;
    Clients clients_;
};
