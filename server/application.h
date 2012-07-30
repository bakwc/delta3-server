#pragma once

#include <QCoreApplication>
#include <QTextCodec>

namespace delta3
{
    class Server;

    /**
     * Класс отвечающий за запуск приложения.
     *
     * На данный момент в нём нет практически никакой функциональности,
     * однако в нём планируется организовать парсинг аргументов командной
     * строки, а так же функциональность, которая не относится
     * непосредственно к серверной части. Внутри этого класса создаётся
     * объект класса Server - большая часть функциональности приложения
     * сосредаточена в нём.
     */
    class Application: public QCoreApplication
    {
        Q_OBJECT
    public:
        Application(int& argc, char* argv[]);
        ~Application ();

        bool start();

    private:
        Server *_server;
    };
}
