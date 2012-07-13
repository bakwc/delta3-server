#pragma once

#include <QCoreApplication>
#include <QTextCodec>

namespace delta3
{
    class Server;

    class Application: public QCoreApplication
    {
        Q_OBJECT
    public:
        Application( int& argc, char* argv[] );
        ~Application ();

        bool start();

    private:
        Server *server_;
    };
}
