#pragma once

#include <QtGlobal>

namespace delta3
{
    const qint32 DEFAULT_TIMER_INTERVAL = 3000;

    const qint8 CSPYP1_PROTOCOL_ID = 1;
    const qint8 CSPYP1_PROTOCOL_VERSION = 2;
    const qint8 CSPYP2_PROTOCOL_ID = 1;
    const qint8 CSPYP2_PROTOCOL_VERSION = 2;

    enum Cspyp1Command
    {
        CMD1_AUTH=1,
        CMD1_ADM,
        CMD1_LIST,
        CMD1_PING,
        CMD1_RLIST,
        CMD1_TRANSMIT,
        CMD1_DISCONNECT,
    };

    enum Cspyp2Command
    {
        CMD2_LIST=1,
        CMD2_MODES,
        CMD2_ACTIVATE,
        CMD2_DEACTIVATE,
        CMD2_TRANSMIT,
    };
}
