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

    /**
     * Основной класс, обеспечивающий открытие порта и приём входящих соединений.
     *
     * Внутри него хранится ассоциативный массив клиентов, каждый раз при
     * подключении нового клиента он размещается в этом массиве. Так же в
     * сервере реализован метод, отвечающий за проверку связи с клиентами
     * (отправляющий пинг запросы), и некоторые вспомогательные функции,
     * возвращающие информацию, которой обладает сервер и которую по
     * смыслу лучше реализовать здесь. Это функция, возвращающая список
     * клиентов в специальном формате, функция возвращающая клиента по
     * его id, функция пересылающая список клиентов администраторам, и
     * некоторые другие.
     */
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
