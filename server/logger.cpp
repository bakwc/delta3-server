#include <QString>
#include <QObject>
#include "logger.h"

namespace delta3 {

    const QString Logger::infoPrefix     = QObject::tr(" [I]: ");
    const QString Logger::debugPrefix    = QObject::tr(" [D]: ");
    const QString Logger::criticalPrefix = QObject::tr(" [C]: ");
    const QString Logger::warningPrefix  = QObject::tr(" [W]: ");
    const QString Logger::fatalPrefix    = QObject::tr(" [F]: ");

    void Logger::toTextStream(QTextStream* stream) {
        QString outString = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
        outString.append(infoPrefix);
        outString.append(buffer);
        if (stream != 0) {
            (*stream) << outString << endl;
            (stream)->flush();
        } else {
            qWarning() << "Logger QTextStream is not initialized.";
        }
    }

    void Logger::closeLogFile() {
        if (_file != 0 && _file->isOpen())
            _file->close();
    }

    void Logger::openLogFile(const QString& fileName) {
        this->_fileName = fileName;
        _file = new QFile(fileName);
        if (_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            _fout = new QTextStream(_file);
        } else {
            qWarning() << "Error opening log file '" << fileName << "'.";
        }
    }

    void Logger::setDefaultStream( OutputStream o)
    {
        if (o == COUT)
           _defaultStream=_cout;
        else if (o == FILE) {
           if (_fout != 0)
              _defaultStream = _fout;
           else
              qWarning() << "Can't change default stream for Logger";
        }

    }

    Logger::~Logger() {
        closeLogFile();
    }

}
