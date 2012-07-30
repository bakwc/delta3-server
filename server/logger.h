#pragma once

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

    Logger() : _cout(new QTextStream(stdout,QIODevice::WriteOnly)), _fout(0), _file(0) { _defaultStream = _cout; }
    ~Logger();

    inline LogMessage message() { return LogMessage(&buffer); }

    inline void toCout()    { toTextStream(_cout); }
    inline void toLogFile() { toTextStream(_fout); }
    inline void write() { toTextStream(_defaultStream); buffer = "";}

    inline static char* toChar(QString str) { return str.toLocal8Bit().data(); }
    inline static const QString ipToStr(qint64 addr) { return QHostAddress(addr).toString(); }
    inline static const QString hostAddressToStr(QHostAddress& addr) { return addr.toString(); }

    void setDefaultStream( OutputStream o);
    void openLogFile(const QString& fileName);
    void closeLogFile();

private:
    QString buffer;
    QTextStream *_cout;
    QTextStream *_fout;
    QString _fileName;
    QFile *_file;
    QTextStream *_defaultStream;

    void toTextStream(QTextStream* stream);

};
} /* namespace delta3 */
