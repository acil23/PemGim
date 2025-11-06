#include "CharacterSelectScene.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "TextRenderer.hpp"
#include "Game.hpp"

void CharacterSelectScene::onEnter(Game* game) {
    gamePtr = game;
    change = false;
    nextScene = "";
    selectedIndex = 0;

    // --- 1. definisi kandidat karakter ---
    // Catatan stat (sementara bisa kamu tuning lagi)
    candidates[0] = ChosenCharacterData{
        "Ali bin Abi Thalib",
        90,     // maxHP lebih kecil tapi serang tinggi
        20,     // baseAttack
        0       // frame index di spritesheet
    };
    candidates[1] = ChosenCharacterData{
        "Hamzah bin Abdul Muthalib",
        110,    // maxHP besar (tank)
        18,     // baseAttack
        1
    };
    candidates[2] = ChosenCharacterData{
        "Ubaidah bin Harits",
        100,    // balanced
        16,     // baseAttack
        2
    };

    // --- 2. load texture spritesheet (choose.png) ---
    SDL_Surface* surf = IMG_Load("../assets/images/choose.png");
    if (!surf) {
        std::cerr << "[CharacterSelectScene] Failed to load choose.png: "
                  << IMG_GetError() << "\n";
        chooseTexture = nullptr;
        frameW = 64;
        frameH = 64;
    } else {
        chooseTexture = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), surf);

        // Asumsi spritesheet = 3 karakter berdiri sejajar horizontal
        frameH = surf->h;
        frameW = surf->w / 3;

        SDL_FreeSurface(surf);
    }

    std::cout << "[CharacterSelectScene] Masuk pemilihan karakter.\n";
    std::cout << "LEFT/RIGHT atau 1/2/3, ENTER untuk pilih. ESC untuk batal.\n";
}

void CharacterSelectScene::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            change = true;
            nextScene = "menu";
            std::cout << "[CharacterSelectScene] Kembali ke menu.\n";
        }
        else if (key == SDLK_LEFT) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (key == SDLK_RIGHT) {
            if (selectedIndex < 2) selectedIndex++;
        }
        else if (key == SDLK_1) {
            selectedIndex = 0;
        }
        else if (key == SDLK_2) {
            selectedIndex = 1;
        }
        else if (key == SDLK_3) {
            selectedIndex = 2;
        }
        else if (key == SDLK_RETURN || key == SDLK_SPACE) {
            // Simpan pilihan ke Game dan lanjut duel
            if (gamePtr) {
                gamePtr->setChosenCharacter(candidates[selectedIndex]);
            }
            change = true;
            nextScene = "duel";
            std::cout << "[CharacterSelectScene] Memilih: "
                      << candidates[selectedIndex].displayName << "\n";
        }
    }
}

void CharacterSelectScene::update(float /*deltaTime*/) {
    // no animation for now
}

void CharacterSelectScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    // --- ambil ukuran layar saat ini (biar responsif fullscreen) ---
    int winW = 800;
    int winH = 480;
    if (gamePtr) {
        gamePtr->getWindowSize(winW, winH);
    }

    // Background hitam abu
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Judul + instruksi
    text->drawText(renderer,
        "Pilih Pejuang Mubarazah",
        20, 20,
        SDL_Color{255,255,255,255});

    text->drawText(renderer,
        "Gunakan LEFT/RIGHT atau [1][2][3], ENTER untuk konfirmasi.",
        20, 50,
        SDL_Color{200,200,200,255});

    // --- Render hanya karakter terpilih, tepat di tengah layar ---
    if (chooseTexture) {
        // ambil frame index dari karakter aktif
        int frameIndex = candidates[selectedIndex].spriteIndex;

        SDL_Rect src {
            frameIndex * frameW,
            0,
            frameW,
            frameH
        };

        // scale biar keliatan lebih besar
        int scale = 2;

        // posisi agar sprite terpilih tepat di tengah
        SDL_Rect dst;
        dst.w = frameW * scale;
        dst.h = frameH * scale;
        dst.x = (winW - dst.w) / 2;
        dst.y = (winH - dst.h) / 2;

        SDL_RenderCopy(renderer, chooseTexture, &src, &dst);

        // kotak sorotan kuning di sekitar karakter
        SDL_Rect outline = dst;
        outline.x -= 4;
        outline.y -= 4;
        outline.w += 8;
        outline.h += 8;

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &outline);
        SDL_RenderDrawRect(renderer, &outline);
    }

    // --- Tampilkan info karakter aktif di sudut bawah kiri ---
    const ChosenCharacterData& ch = candidates[selectedIndex];

    int infoY = winH - 120;
    text->drawText(renderer,
        ch.displayName,
        20,
        infoY,
        SDL_Color{255,255,255,255});

    text->drawText(renderer,
        "HP: " + std::to_string(ch.maxHP),
        20,
        infoY + 24,
        SDL_Color{200,200,200,255});

    text->drawText(renderer,
        "Serangan dasar: " + std::to_string(ch.baseAttack),
        20,
        infoY + 48,
        SDL_Color{200,200,200,255});
}
