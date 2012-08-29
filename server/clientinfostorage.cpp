#include "clientinfostorage.h"
#include "server.h"

ClientInfoStorage::ClientInfoStorage(QObject *parent):
    QObject(parent),
    _changed(false)
{
}

void ClientInfoStorage::updateClient(ClientInfo client)
{
    client.lastSeen=time(NULL);
    auto i=_clients.find(client.hash);
    if (i==_clients.end())
    {
        client.caption=QString("%1 (%2)")
                .arg(QHostAddress(client.ip).toString())
                .arg(client.os);
        _clients.insert(client.hash,client);
    } else
    {
        i->hash=client.hash;
        i->device=client.device;
        i->ip=client.ip;
        i->os=client.os;
    }
    _changed=true;
}

void ClientInfoStorage::updateCaption(const QByteArray& hash, const QString& caption)
{
    auto i=_clients.find(hash);
    if (i==_clients.end())
        return;
    i->caption=caption;
    _changed=true;
}

QString ClientInfoStorage::getCaption(const QByteArray& hash)
{
    auto i=_clients.find(hash);
    if (i==_clients.end())
        return "";
    return i->caption;
}

void ClientInfoStorage::setCaption(const QByteArray& hash, const QString& caption)
{
    auto i=_clients.find(hash);
    if (i==_clients.end())
        return;
    i->caption=caption;
    _changed=true;
}

void ClientInfoStorage::save()
{
    if (!_changed)
        return;
    QFile outFile(settings()->value("general/datafile", delta3::STORAGE_FILE).toString());
    outFile.open(QIODevice::WriteOnly);
    QTextStream out(&outFile);

    for (auto i=_clients.begin();i!=_clients.end();i++)
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
    _changed=false;
}

void ClientInfoStorage::load()
{
    QFile inputFile(settings()->value("general/datafile", delta3::STORAGE_FILE).toString());

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
        _clients.insert(info.hash,info);
        //qDebug() << "Client" << info.caption << "loaded";
    }
    inputFile.close();
}

QSettings* ClientInfoStorage::settings()
{
    return ((delta3::Server*)(parent()))->settings();
}
