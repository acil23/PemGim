#include "Game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Game game;
    // UBAH DI SINI: Ganti 1280, 720 menjadi 1280, 720
    if (!game.init("Sejarah RPG - Perang Badar", 1280, 720, false)) {
        std::cerr << "Game init failed\n";
        return 1;
    }

    game.run();

    return 0;
}