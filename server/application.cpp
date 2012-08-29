#include "application.h"

#include "server.h"

namespace delta3
{
    static const QString CONFIG_FILE = "config.ini";

    Application::Application( int& argc, char* argv[] ):
        QCoreApplication( argc, argv )
    {
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
        _settings = new QSettings(CONFIG_FILE, QSettings::IniFormat);
        _settings->setParent(this);
        _server = new Server(this);
    }

    Application::~Application()
    {
        qDebug() << tr("Terminating..");
        _settings->sync();
    }

    bool Application::start()
    {
        return _server->start();
    }

    QSettings* Application::getSettings()
    {
        return _settings;
    }

}
