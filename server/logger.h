#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QHostAddress>
#include <QFile>
#include <QDebug>


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
    ~Logger() {
        closeLogFile();
    }

    inline QDebug message() { return QDebug(&buffer); }

    inline void toCout()    { toTextStream(cout_); }
    inline void toLogFile() { toTextStream(fout_); }

    inline static const char* tr(const char* str) { return QObject::tr(str).toLocal8Bit().data(); }
    inline static const char* ipToStr(qint64 addr) { return QHostAddress(addr).toString().toLocal8Bit().data(); }

    inline void setDefaultStream( OutputStream o)
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

    inline void write() { toTextStream(defaultStream); buffer = "";}

    void openLogFile(const QString& fileName) {
        this->fileName = fileName;
        file = new QFile(fileName);
        if (file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            fout_ = new QTextStream(file);
        } else {
            qWarning() << "Error opening log file '" << fileName << "'.";
        }

    }

    void closeLogFile() {
        if (file != 0 && file->isOpen())
            file->close();
    }

private:
    QString buffer;
    QTextStream *cout_;
    QTextStream *fout_;
    QString fileName;
    QFile *file;
    QTextStream *defaultStream;

    void toTextStream(QTextStream* stream) {
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

};
} /* namespace delta3 */

#endif // LOGGER_H
