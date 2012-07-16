#pragma once
#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include "defines.h"
#include "utils.h"

namespace delta3
{
    qint8 getProtoId(const QByteArray& buffer);
    qint8 getProtoVersion(const QByteArray& buffer);
    Cspyp1Command getCommand(const QByteArray& buffer);
    QByteArray getClientHash(const QByteArray& buffer);
    QString getClientOs(const QByteArray& buffer);
    QString getClientDevice(const QByteArray& buffer);
    QString getAdminLogin(const QByteArray& buffer);
    QString getAdminPassword(const QByteArray& buffer);
    qint32 getPacketLength(const QByteArray& buffer);
    qint16 getClientId(const QByteArray& buffer);
    QByteArray getPacketData(const QByteArray& buffer);
    QString getClientCaption(const QByteArray& buffer);
}
