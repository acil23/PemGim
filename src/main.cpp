#include "Game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Game game;
    if (!game.init("Sejarah RPG - Perang Badar", 800, 480, false)) {
        std::cerr << "Game init failed\n";
        return 1;
    }

    game.run();

    return 0;
}
