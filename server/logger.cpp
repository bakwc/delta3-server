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
        if (file != 0 && file->isOpen())
            file->close();
    }

    void Logger::openLogFile(const QString& fileName) {
        this->fileName = fileName;
        file = new QFile(fileName);
        if (file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            fout_ = new QTextStream(file);
        } else {
            qWarning() << "Error opening log file '" << fileName << "'.";
        }
    }

    void Logger::setDefaultStream( OutputStream o)
    {
        if (o == COUT)
           defaultStream=cout_;
        else if (o == FILE) {
           if (fout_ != 0)
              defaultStream = fout_;
           else
              qWarning() << "Can't change default stream for Logger";
        }

    }

    Logger::~Logger() {
        closeLogFile();
    }

}
