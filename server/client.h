#pragma once
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QRegExp>
#include <ctime>

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
    void send(const QString &cmd) const;
    void sendPing() const;
    qint32 getId() const;
    QString getIdHash() const;
    ClientStatus getStatus() const;
    quint32 getLastSeen() const;
    void setSeen();
    void disconnectFromHost();
    void sendList(const QString &list);
private slots:
    void onDataReceived();
private:
    void parseData(const QString &data);
    bool parseClientAuth(const QString& data);
    bool parseAdminAuth(const QString& data);
    bool parseList(const QString& data);
    bool parseTransmit(const QString& data);
    bool parseDisconnect(const QString& data);
    Server *getServer();
private:
    quint32 lastSeen_;   //timestamp
    ClientStatus status_;
    QTcpSocket* socket_;
    QString clientIdHash_;
};
