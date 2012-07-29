#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

namespace delta3 {
class LogMessage
{
    struct Stream {
        Stream(QString *string) : ts(string, QIODevice::WriteOnly), ref(1), type(QtDebugMsg), space(true), message_output(false) {}
        QTextStream ts;
        QString buffer;
        int ref;
        QtMsgType type;
        bool space;
        bool message_output;
    } *stream;
public:
    inline LogMessage(QString *string) : stream(new Stream(string)) {}
    inline LogMessage(const LogMessage &o):stream(o.stream) { ++stream->ref; }
    inline LogMessage &operator=(const LogMessage &other);
    inline ~LogMessage() {
        if (!--stream->ref) {
            if(stream->message_output) {
               qt_message_output(stream->type, stream->buffer.toLocal8Bit().data());
            }
            delete stream;
        }
    }

    inline LogMessage &operator<<(QChar t) { stream->ts << '\'' << t << '\''; return *this; }
    inline LogMessage &operator<<(QBool t) { stream->ts << (bool(t != 0) ? "true" : "false"); return *this; }
    inline LogMessage &operator<<(bool t) { stream->ts << (t ? "true" : "false"); return *this; }
    inline LogMessage &operator<<(char t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(signed short t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(unsigned short t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(signed int t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(unsigned int t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(signed long t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(unsigned long t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(qint64 t)
        { stream->ts << QString::number(t); return *this; }
    inline LogMessage &operator<<(quint64 t)
        { stream->ts << QString::number(t); return *this; }
    inline LogMessage &operator<<(float t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(double t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(const char* t) { stream->ts << QString::fromAscii(t); return *this; }
    inline LogMessage &operator<<(const QString & t) { stream->ts << t;  return *this; }
    inline LogMessage &operator<<(const QStringRef & t) { return operator<<(t.toString()); }
    inline LogMessage &operator<<(const QLatin1String &t) { stream->ts << t.latin1(); return *this; }
    inline LogMessage &operator<<(const QByteArray & t) { stream->ts  << t ; return *this; }
    inline LogMessage &operator<<(const void * t) { stream->ts << t; return *this; }
    inline LogMessage &operator<<(QTextStreamFunction f) {
        stream->ts << f;
        return *this;
    }

    inline LogMessage &operator<<(QTextStreamManipulator m)
    { stream->ts << m; return *this; }
};

}


#endif // LOGMESSAGE_H
