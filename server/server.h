/*!
 * @file server.h
 * Write description of source file here for dOxygen.
 *
 * @brief Can use "brief" tag to explicitly generate comments for file documentation.
 *
 * @author Me
   [Note: depricated practice as CM systems like Subversion will maintain author info and blame logs.]
 * @version 1.69
   [Note: depricated practice as CM systems like Subversion will maintain file history and revision numbers.]
 */
// $Log$

#pragma once

#include <QObject>
#include <QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QMap>
#include <QTimerEvent>
#include <QSettings>

#include "clientinfostorage.h"
#include "logger.h"
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
        void setAdminTalkingWithClient(qint16 clientId, qint16 adminId);
        inline Logger& getLogger() {return _logger;}
        QSettings *settings();

    private slots:
        void onNewConnection();

    private:
        void timerEvent( QTimerEvent* event );

    private:
        Logger _logger;
        QTcpServer* _tcpServer;
        Clients _clients;
        ClientInfoStorage *_storage;
    };
}
