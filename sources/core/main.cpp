#include <iostream>

#include "core/core.hpp"

int
main(int argc, char* argv[])
{
    std::cout << "LAMPA" << std::endl;

    core::Core app;
    app.run();

    std::cout << "91" << std::endl;

    return 0;
}
