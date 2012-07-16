#include "netextract.h"

namespace delta3
{
    qint8 getProtoId(const QByteArray& buffer)
    {
        return (qint8)(buffer[0]);
    }

    qint8 getProtoVersion(const QByteArray& buffer)
    {
        return (qint8)(buffer[1]);
    }

    Cspyp1Command getCommand(const QByteArray& buffer)
    {
        return (Cspyp1Command)(buffer[2]);
    }

    QByteArray getClientHash(const QByteArray& buffer)
    {
        return buffer.mid(3,16);
    }

    QString getClientOs(const QByteArray& buffer)
    {
        return buffer.mid(19,20);
    }

    QString getClientDevice(const QByteArray& buffer)
    {
        return buffer.mid(39,20);
    }

    QString getAdminLogin(const QByteArray& buffer)
    {
        return buffer.mid(3,22);
    }

    QString getAdminPassword(const QByteArray& buffer)
    {
        return buffer.mid(25,22);
    }

    qint32 getPacketLength(const QByteArray& buffer)
    {
        return fromBytes<qint32>(buffer.mid(5,4));
    }

    qint16 getClientId(const QByteArray& buffer)
    {
        return fromBytes<qint16>(buffer.mid(3,2));
    }

    QString getClientCaption(const QByteArray& buffer)
    {
        return buffer.mid(5,30);
    }

    QByteArray getPacketData(const QByteArray& buffer)
    {
        return buffer.mid(9,getPacketLength(buffer));
    }
}
