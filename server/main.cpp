#include "application.h"

#include <cstdlib>

int main(int argc, char *argv[])
{
    delta3::Application app(argc, argv);

    if (!app.start())
    {
        return EXIT_FAILURE;
    }
    
    return app.exec();
}
