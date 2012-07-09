#pragma once
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QRegExp>

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
    qint32 getId() const;
    QString getIdHash() const;
    ClientStatus getStatus() const;
private slots:
    void onDataReceived();
private:
    void parseData(const QByteArray& data);
    bool parseClientAuth(const QByteArray& data);
    bool parseAdminAuth(const QByteArray& data);
    bool parseList(const QByteArray& data);
    bool parseTransmit(const QByteArray& data);
    Server *getServer();
private:
    ClientStatus status_;
    QTcpSocket* socket_;
    QString clientIdHash_;
};
