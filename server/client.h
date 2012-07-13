#pragma once
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QRegExp>
#include <ctime>
#include "defines.h"

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
    void sendPing() const;
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
    void parseClientAuth();
    void parseAdminAuth();
    void parseList();
    void parseTransmit();
    void parseDisconnect();
    void parsePing();

    Server *getServer();

    qint8 getProtoId();
    qint8 getProtoVerstion();
    Cspyp1Command getCommand();
    QByteArray getClientHash();
    QString getAdminLogin();
    QString getAdminPassword();
    qint32 getPacketLength();
    qint16 getClientId();
    QByteArray getPacketData();

private:
    struct ClientInfo
    {
        QByteArray hash;
    };

    struct AdminInfo
    {
        QString login;
        QString pass;
    };

private:
    quint32 lastSeen_;   //timestamp
    ClientStatus status_;
    QTcpSocket* socket_;
    void* clientInfo_;  // pointer to info
    QByteArray buf_;
};

