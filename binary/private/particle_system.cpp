#include "Engine.hpp"

int
main()
{
    try {
        Engine engine;

        engine.init();
        engine.run();
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
    return (0);
}