#pragma once

#include <QObject>
#include <QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QMap>
#include <QTimerEvent>

#include "clientinfostorage.h"
#include "defines.h"

namespace delta3
{
    class Client;

    typedef QMap<qint32,Client*> Clients;

    class Server: public QObject
    {
        Q_OBJECT
    public:
        Server(QObject* parent = 0);
        ~Server();

        bool start();

        QByteArray listConnectedClients();
        Clients::iterator searchClient(qint32 clientId);
        Clients::iterator clientEnd();
        void resendListToAdmins();
        void setClientCaption(qint16 clientId, const QString& caption);

    private slots:
        void onNewConnection();

    private:
        void timerEvent( QTimerEvent* event );

    private:
        QTcpServer* tcpServer_;
        Clients clients_;
        ClientInfoStorage *storage_;
    };
}
