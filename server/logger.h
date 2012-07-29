#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QHostAddress>
#include <QFile>
#include <QDebug>
#include "logmessage.h"


namespace delta3 {
class Logger {

public:

    static const QString infoPrefix;
    static const QString debugPrefix;
    static const QString criticalPrefix;
    static const QString warningPrefix;
    static const QString fatalPrefix;

    enum OutputStream { COUT, FILE };

    Logger() : cout_(new QTextStream(stdout,QIODevice::WriteOnly)), fout_(0), file(0) { defaultStream = cout_; }
    ~Logger();

    inline LogMessage message() { return LogMessage(&buffer); }

    inline void toCout()    { toTextStream(cout_); }
    inline void toLogFile() { toTextStream(fout_); }
    inline void write() { toTextStream(defaultStream); buffer = "";}

    inline static char* toChar(QString str) { return str.toLocal8Bit().data(); }
    inline static const QString ipToStr(qint64 addr) { return QHostAddress(addr).toString(); }
    inline static const QString hostAddressToStr(QHostAddress& addr) { return addr.toString(); }

    void setDefaultStream( OutputStream o);
    void openLogFile(const QString& fileName);
    void closeLogFile();

private:
    QString buffer;
    QTextStream *cout_;
    QTextStream *fout_;
    QString fileName;
    QFile *file;
    QTextStream *defaultStream;

    void toTextStream(QTextStream* stream);

};
} /* namespace delta3 */

#endif // LOGGER_H
