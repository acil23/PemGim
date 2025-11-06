#include "MenuScene.hpp"
#include <SDL.h>
#include <iostream>
#include "TextRenderer.hpp"

void MenuScene::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            change = true;
            next = "";
        }
        else if (key == SDLK_1) {
            change = true;
            next = "badar_intro";
            std::cout << "[MenuScene] Player memilih Perang Badar\n";
        }
    }
}

void MenuScene::update(float /*deltaTime*/) {
    // no-op
}

void MenuScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    // background gelap
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    text->drawText(renderer,
        "Sejarah RPG",
        20, 20,
        SDL_Color{255,255,255,255});

    text->drawText(renderer,
        "1. Perang Badar",
        20, 60,
        SDL_Color{200,200,200,255});

    text->drawText(renderer,
        "ESC untuk keluar",
        20, 100,
        SDL_Color{180,180,180,255});
}
