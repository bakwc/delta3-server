#include "clientinfostorage.h"

ClientInfoStorage::ClientInfoStorage(QObject *parent):
    QObject(parent),
    changed_(false)
{
}

void ClientInfoStorage::updateClient(ClientInfo client)
{
    client.lastSeen=time(NULL);
    auto i=clients_.find(client.hash);
    if (i==clients_.end())
    {
        client.caption=QString("%1 (%2)")
                .arg(QHostAddress(client.ip).toString())
                .arg(client.os);
        clients_.insert(client.hash,client);
    } else
    {
        i->hash=client.hash;
        i->device=client.device;
        i->ip=client.ip;
        i->os=client.os;
    }
    changed_=true;
}

void ClientInfoStorage::updateCaption(const QByteArray& hash, const QString& caption)
{
    auto i=clients_.find(hash);
    if (i==clients_.end())
        return;
    i->caption=caption;
    changed_=true;
}

QString ClientInfoStorage::getCaption(const QByteArray& hash)
{
    auto i=clients_.find(hash);
    if (i==clients_.end())
        return "";
    return i->caption;
}

void ClientInfoStorage::setCaption(const QByteArray& hash, const QString& caption)
{
    auto i=clients_.find(hash);
    if (i==clients_.end())
        return;
    i->caption=caption;
    changed_=true;
}

void ClientInfoStorage::save()
{
    if (!changed_)
        return;
    QFile outFile(delta3::STORAGE_FILE);
    outFile.open(QIODevice::WriteOnly);
    QTextStream out(&outFile);

    QByteArray lol;

    for (auto i=clients_.begin();i!=clients_.end();i++)
    {
        out << QString("%1:%2:%3:%4:%5:%6\n")
               .arg((QString)(i->hash.toHex()))
               .arg((i->ip).toString())
               .arg(i->os)
               .arg(i->device)
               .arg(i->lastSeen)
               .arg(i->caption);
     }
    outFile.close();
    changed_=false;
}

void ClientInfoStorage::load()
{
    QFile inputFile(delta3::STORAGE_FILE);

    if (!inputFile.exists())
        return;

    inputFile.open(QIODevice::ReadOnly);

    QString line;
    while (!inputFile.atEnd())
    {
        line=inputFile.readLine();
        auto params=line.split(":");
        if (params.size()!=6)
            continue;

        ClientInfo info;
        info.hash=QByteArray::fromHex(params[0].toLocal8Bit());
        info.ip=QHostAddress(params[1]);
        info.os=params[2];
        info.device=params[3];
        info.lastSeen=params[4].toInt();
        info.caption=params[5].trimmed();
        clients_.insert(info.hash,info);
        qDebug() << "Client" << info.caption << "loaded";
    }
    inputFile.close();
}
