#pragma once
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QObject>

class Client: public QObject
{
    Q_OBJECT
public:
    Client(QTcpSocket *socket, QObject *parent=0);
    void send(const QByteArray &cmd);
    qint32 getId();
private slots:
    void onDataReceived();
public:
    enum ClientStatus
    {
        ST_DISCONNECTED,
        ST_CONNECTED,
        ST_CLIENT,
        ST_ADMIN
    };
private:
    ClientStatus status_;
    QTcpSocket* socket_;
    QString hash_;
};
