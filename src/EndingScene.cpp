#include "EndingScene.hpp"
#include <SDL.h>
#include <iostream>
#include "TextRenderer.hpp"
#include "Game.hpp"

void EndingScene::onEnter(Game* game) {
    gamePtr = game;
    change = false;
    next = "";

    std::cout << "\n[EndingScene]\n";
    std::cout << "Perang Badar dimenangkan oleh kaum Muslimin.\n";
    std::cout << "Beberapa sahabat syahid, dan banyak pemimpin Quraisy gugur.\n";
    std::cout << "Tekan ENTER untuk kembali ke menu utama.\n";
}

void EndingScene::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_RETURN) {
            change = true;
            next   = "menu";
            std::cout << "[EndingScene] Kembali ke menu.\n";
        }
    }
}

void EndingScene::update(float /*deltaTime*/) {
    // no-op
}

void EndingScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 80, 255);
    SDL_RenderClear(renderer);

    int y = 20;
    text->drawText(renderer,
        "Akhir Perang Badar",
        20, y,
        SDL_Color{255,255,255,255});
    y += 40;

    text->drawText(renderer,
        "Kaum Muslimin meraih kemenangan.",
        20, y,
        SDL_Color{220,220,255,255});
    y += 25;

    text->drawText(renderer,
        "Sejumlah sahabat gugur sebagai syuhada.",
        20, y,
        SDL_Color{220,220,255,255});
    y += 25;

    text->drawText(renderer,
        "Banyak pemimpin Quraisy tewas.",
        20, y,
        SDL_Color{220,220,255,255});
    y += 40;

    text->drawText(renderer,
        "Tekan ENTER untuk kembali ke menu.",
        20, y,
        SDL_Color{200,200,200,255});
}
