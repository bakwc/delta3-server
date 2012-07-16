#include "clientinfostorage.h"

ClientInfoStorage::ClientInfoStorage(QObject *parent):
    QObject(parent)
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
}

void ClientInfoStorage::updateCaption(const QByteArray& hash, const QString& caption)
{
    auto i=clients_.find(hash);
    if (i==clients_.end())
        return;
    i->caption=caption;
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
}

void ClientInfoStorage::save()
{

}

void ClientInfoStorage::load()
{

}
