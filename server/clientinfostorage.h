#pragma once
#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QHostAddress>
#include <QMap>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <ctime>
#include "defines.h"


class ClientInfoStorage: public QObject
{
    Q_OBJECT
public:
    struct ClientInfo
    {
        QByteArray hash;
        QString os;
        QString device;
        QString caption;
        QHostAddress ip;
        qint32 lastSeen; // timestamp
    };
public:
    ClientInfoStorage(QObject *parent=0);
    void updateClient(ClientInfo client);
    void updateCaption(const QByteArray& hash, const QString& caption);
    QString getCaption(const QByteArray& hash);
    void setCaption(const QByteArray& hash, const QString& caption);
    void save();
    void load();

private:
    QMap<QByteArray, ClientInfo> clients_;
    bool changed_;
};

