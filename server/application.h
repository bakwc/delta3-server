#pragma once
#include <QCoreApplication>
#include "server.h"

class Application: public QCoreApplication
{
    Q_OBJECT
public:
    Application( int& argc, char* argv[] );
    ~Application ();
    bool init();
    bool startServer();
private:
    Server *server_;
};
