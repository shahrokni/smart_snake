#include "./game.h"
#include <stdexcept>
#include <iostream>

int main()
{
    try
    {
        SnakeGame snake_game;
        snake_game.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}