#include "application.h"

Application::Application( int& argc, char* argv[] ):
    QCoreApplication( argc, argv ),
    server_(new Server(this))
{
}

Application::~Application()
{
}

bool Application::init()
{
    if (!startServer()) return false;
    return true;
}

bool Application::startServer()
{
    return server_->start();
}
