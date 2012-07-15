#pragma once

#include <QtGlobal>

namespace delta3
{
    const qint32 DEFAULT_TIMER_INTERVAL = 3000;

    const qint8 CSPYP1_PROTOCOL_ID = 1;
    const qint8 CSPYP1_PROTOCOL_VERSION = 3;
    const qint8 CSPYP2_PROTOCOL_ID = 1;
    const qint8 CSPYP2_PROTOCOL_VERSION = 2;

    const qint16 CMD1_AUTH_SIZE = 59;   // Authorisation packet size
    const qint16 CMD1_ADM_SIZE = 47;    // Admin authorisation packet size
    const qint16 CMD1_CLIENT_INFO_SIZE = 92;    // Client info struct size
    const qint16 CMD1_SETINFO_SIZE = 35;    // Setinfo size

    enum Cspyp1Command
    {
        CMD1_AUTH=1,
        CMD1_ADM,
        CMD1_LIST,
        CMD1_PING,
        CMD1_RLIST,
        CMD1_TRANSMIT,
        CMD1_DISCONNECT,
        CMD1_SETINFO
    };

    enum Cspyp2Command
    {
        CMD2_LIST=1,
        CMD2_MODES,
        CMD2_ACTIVATE,
        CMD2_DEACTIVATE,
        CMD2_TRANSMIT
    };

    enum ClientStatus
    {
        ST_DISCONNECTED,
        ST_CONNECTED,
        ST_CLIENT,
        ST_ADMIN
    };
}
