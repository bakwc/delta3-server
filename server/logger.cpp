#include <QString>
#include <QObject>
#include "logger.h"

namespace delta3 {

    const QString Logger::infoPrefix     = QString(tr(" [I]: "));
    const QString Logger::debugPrefix    = QString(tr(" [D]: "));
    const QString Logger::criticalPrefix = QString(tr(" [C]: "));
    const QString Logger::warningPrefix  = QString(tr(" [W]: "));
    const QString Logger::fatalPrefix    = QString(tr(" [F]: "));

}
