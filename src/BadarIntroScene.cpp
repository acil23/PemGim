#include "BadarIntroScene.hpp"
#include <SDL.h>
#include <iostream>

#include "TextRenderer.hpp"
#include "Game.hpp"

void BadarIntroScene::onEnter(Game* game) {
    gamePtr = game;
    npcFrame = 0;
    change = false;
    next = "";

    // siapkan naskah dialog
    // nanti kamu bisa ganti ini jadi data sejarah yang lebih akurat
    std::vector<std::string> lines = {
        "Perang Badar: 17 Ramadhan tahun 2 Hijriyah.",
        "Kaum Muslimin sekitar 313 orang, berhadapan dengan Quraisy yang berjumlah sekitar 1000.",
        "Rasulullah SAW memimpin langsung. Banyak sahabat utama ikut.",
        "Pertarungan dibuka dengan mubarazah: Ali bin Abi Thalib, Hamzah bin Abdul Muthalib, dan Ubaidah bin Harits maju menghadapi para jawara Quraisy.",
        "Bersiaplah. Setelah ini kamu akan memasuki duel."
    };

    dialog.setLines(lines);

    std::cout << "\n[BadarIntroScene]\n";
    std::cout << "SPACE/ENTER untuk lanjut dialog.\n";
    std::cout << "ESC untuk kembali menu.\n";
}

void BadarIntroScene::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            change = true;
            next = "menu";
            std::cout << "[BadarIntroScene] Kembali ke menu...\n";
        }
        else if (key == SDLK_SPACE || key == SDLK_RETURN) {
            // lanjut dialog
            bool masihAda = dialog.next();
            // flip frame npc
            npcFrame = (npcFrame == 0 ? 1u : 0u);

            if (!masihAda) {
                // dialog habis -> lanjut ke duel
                change = true;
                next = "char_select";
                std::cout << "[BadarIntroScene] Dialog selesai. Masuk duel...\n";
            }
        }
    }
}

void BadarIntroScene::update(float /*deltaTime*/) {
    // Untuk sekarang ga ada animasi waktu jalan, hanya pas next dialog
}

void BadarIntroScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    // ambil ukuran window saat ini
    int winW = 0;
    int winH = 0;
    if (gamePtr) {
        gamePtr->getWindowSize(winW, winH);
    } else {
        winW = 1280;
        winH = 720;
    }

    // background merah gelap
    SDL_SetRenderDrawColor(renderer, 80, 20, 20, 255);
    SDL_RenderClear(renderer);

    // judul scene di atas
    text->drawText(renderer,
        "Perang Badar - Pendahuluan",
        20, 20,
        SDL_Color{255,255,255,255});

    // render dialog box di bawah pakai ukuran window aktual
    if (gamePtr) {
        dialog.render(
            renderer,
            text,
            gamePtr->getNpcTexture(),
            winW,
            winH,
            npcFrame
        );
    }
}

