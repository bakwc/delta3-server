#pragma once

#include <memory>

#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QSet>
#include <ctime>
#include "defines.h"

class ClientInfoStorage;

namespace delta3
{
    class Server;

    /**
     * Класс, в котором сосредоточена информация о клиенте
     *
     * Так же здесь размещены различные методы, касающиеся обработки данных,
     * приходящих по сети этому клиенту. Клиент может быть неавторизованным
     * клиентом, обычным клиентом или администратором. Среди основных методов,
     * которые расположены в классе Client - функция отправки данных этому
     * клиенту, функция обработки входящих данных, набор функция для парсинга
     * команд протокола различного типа.
     */
    class Client: public QObject
    {
        Q_OBJECT
    public:
        Client(QTcpSocket *socket, ClientInfoStorage *storage, QObject *parent=0);

        // Посылает массив данных на сокет.
        void send(const QByteArray &cmd) const;

        // Генерирует и посылает ping пакет на клиент
        // Если ответ не приходит (проверка в другом месте),
        // клиент считается отключенным.
        void ping() const;

        // У нас айди клиента и админа это
        // декриптор соответствующего ему сокета.
        // Айди нужны для адресации пакетов.
        // Эта функция возвращает декриптор сокета.
        qint16 getId() const;

        // Для различения и сортировки в будущем
        // клиентов, у них есть свой 16 байтный хэш
        QByteArray getIdHash() const;

        // На сервере хранится информация о клиенте
        // Эта функция вернет ОС клиента.
        QString getOs() const;

        // -//-
        // Вернет тип девайса клиента (desktop / mobile  и прочие)
        QString getDevice() const;

        QString getCaption() const;

        // Вернет статус клиента
        ClientStatus getStatus() const;

        // Добавит админа, которому разрешено отвечать клиенту
        void addTalkingWithAdmin(qint16 adminId);

        quint32 getLastSeen() const;
        qint32 getIp() const;
        void setSeen();
        void setCaption(const QString& caption);
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
        void sendPong();
        void parseSetInfo();

    private:
        struct BasicInfo
        {
            virtual ~BasicInfo() {}
        };

        struct ClientInfo : BasicInfo
        {
            QByteArray hash;
            QString os;
            QString deviceType;
            QString caption;
            QSet<qint16> admins; // list of admins, talking with this client
        };

        struct AdminInfo : BasicInfo
        {
            QString login;
            QString pass;
        };

    private:
        Server *getServer() const;
        ClientInfo *getClientInfo() const;
        AdminInfo *getAdminInfo() const;

    private:
        quint32 lastSeen_;   //timestamp
        QTcpSocket* socket_;
        ClientInfoStorage *storage_;
        std::unique_ptr<BasicInfo> clientInfo_;
        ClientStatus status_;
        QByteArray buf_;
    };
}
