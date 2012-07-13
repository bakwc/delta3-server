#pragma once
#include <QtGlobal>

const qint32 DEFAULT_TIMER_INTERVAL = 3000;

const qint8 CSPYP1_PROTOCOL_ID = 1;
const qint8 CSPYP1_PROTOCOL_VERSION = 2;
const qint8 CSPYP2_PROTOCOL_ID = 1;
const qint8 CSPYP2_PROTOCOL_VERSION = 2;


enum Cspyp1Command
{
    CMD1_AUTH=1,
    CMD1_ADM=2,
    CMD1_LIST=3,
    CMD1_PING=4,
    CMD1_RLIST=5,
    CMD1_TRANSMIT=6,
    CMD1_DISCONNECT=7
};

enum Cspyp2Command
{
    CMD2_LIST=1,
    CMD2_MODES=2,
    CMD2_ACTIVATE=3,
    CMD2_DEACTIVATE=4,
    CMD2_TRANSMIT=5
};
