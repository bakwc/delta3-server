#include "application.h"

#include "server.h"

namespace delta3
{
    Application::Application( int& argc, char* argv[] ):
        QCoreApplication( argc, argv ),
        _server(new Server(this))
    {
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    }

    Application::~Application()
    {
    }

    bool Application::start()
    {
        return _server->start();
    }

}
