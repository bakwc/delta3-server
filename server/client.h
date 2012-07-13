#pragma once

#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QRegExp>
#include <ctime>

#include "defines.h"

namespace delta3
{
    enum ClientStatus
    {
        ST_DISCONNECTED,
        ST_CONNECTED,
        ST_CLIENT,
        ST_ADMIN
    };

    class Server;

    class Client: public QObject
    {
        Q_OBJECT
    public:
        Client(QTcpSocket *socket, QObject *parent=0);

        void send(const QByteArray &cmd) const;
        void ping() const;
        qint16 getId() const;
        QByteArray getIdHash() const;
        ClientStatus getStatus() const;
        quint32 getLastSeen() const;
        void setSeen();
        void disconnectFromHost();
        void sendList(const QByteArray &list);

    private slots:
        void onDataReceived();

    private:
        struct CommandTableType
        {
            Cspyp1Command command;
            void (Client::*function)();
        };

        static const CommandTableType CommandTable[];

    private:
        void parseClientAuth();
        void parseAdminAuth();
        void parseList();
        void parseTransmit();
        void parseDisconnect();
        void parsePing();

    private:
        Server *getServer();

    private:
        quint32 lastSeen_;   //timestamp
        ClientStatus status_;
        QTcpSocket* socket_;
        void* clientInfo_;  // pointer to info
        QByteArray buf_;
    };
}
