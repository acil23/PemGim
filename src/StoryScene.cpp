#include "StoryScene.hpp"
#include "Game.hpp"
#include <SDL_image.h>
#include <iostream>
#include <sstream>

StoryScene::StoryScene() : change(false), next(""), bgTexture(nullptr) {}

void StoryScene::setup(const std::string& bgPath, 
                       const std::string& title,
                       const std::vector<std::string>& textLines, 
                       const std::string& nextSceneID) 
{
    backgroundPath = bgPath;
    sceneTitle = title;
    lines = textLines;
    next = nextSceneID;
}

// Helper: Semakin panjang teks, semakin lama waktunya
float StoryScene::calculateReadingTime(const std::string& text) {
    // Base time 1.5 detik + 0.05 detik per huruf
    // Contoh: Teks 100 huruf = 1.5 + 5.0 = 6.5 detik
    return 1.5f + (text.length() * 0.05f);
}

void StoryScene::onEnter(Game* game) {
    gamePtr = game;
    change = false;
    currentLineIndex = 0;
    
    // Reset timer
    autoTimer = 0.0f;
    if (!lines.empty()) {
        currentDuration = calculateReadingTime(lines[0]);
    } else {
        currentDuration = 2.0f;
    }

    if (!backgroundPath.empty()) {
        SDL_Surface* surf = IMG_Load(backgroundPath.c_str());
        if (surf) {
            bgTexture = SDL_CreateTextureFromSurface(game->getRenderer(), surf);
            SDL_FreeSurface(surf);
        }
    }
}

void StoryScene::onExit() {
    if (bgTexture) { SDL_DestroyTexture(bgTexture); bgTexture = nullptr; }
}

void StoryScene::handleEvent(const SDL_Event& e) {
    bool manualSkip = false;

    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_RETURN) {
            manualSkip = true;
        }
    }
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        manualSkip = true;
    }

    if (manualSkip) {
        // Kalau user tekan Spasi, kita "paksa" timer jadi penuh
        // Supaya logika pindah baris dijalankan di frame berikutnya (atau langsung disini)
        
        if (currentLineIndex < lines.size() - 1) {
            currentLineIndex++;
            autoTimer = 0.0f; // Reset timer untuk baris baru
            currentDuration = calculateReadingTime(lines[currentLineIndex]);
        } 
        else {
            change = true; // Scene selesai
        }
    }
}

void StoryScene::update(float deltaTime) {
    if (lines.empty()) return;

    // Tambahkan waktu
    autoTimer += deltaTime;

    // Cek apakah waktu baca untuk baris ini sudah habis?
    if (autoTimer >= currentDuration) {
        
        // Apakah masih ada baris berikutnya?
        if (currentLineIndex < lines.size() - 1) {
            currentLineIndex++;       // Munculkan baris baru
            autoTimer = 0.0f;         // Reset timer
            currentDuration = calculateReadingTime(lines[currentLineIndex]); // Hitung durasi baru
        } 
        else {
            // Jika sudah di baris terakhir, beri jeda ekstra sebelum ganti scene
            // Misal tambah 3 detik lagi biar player sempat baca endingnya
            if (autoTimer >= currentDuration + 3.0f) {
                change = true;
            }
        }
    }
}

// ... Fungsi wrapText sama seperti sebelumnya ...
std::vector<std::string> StoryScene::wrapText(TextRenderer* text, SDL_Renderer* r, const std::string& str, int maxWidth) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string word, line;
    while (iss >> word) {
        std::string test = line.empty() ? word : line + " " + word;
        if (text->measureWidth(r, test) > maxWidth) {
            result.push_back(line);
            line = word;
        } else {
            line = test;
        }
    }
    if (!line.empty()) result.push_back(line);
    return result;
}

void StoryScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    int w, h;
    gamePtr->getWindowSize(w, h);

    if (bgTexture) {
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100); 
    SDL_Rect screenRect = {0, 0, w, h};
    SDL_RenderFillRect(renderer, &screenRect);

    // Judul
    if (!sceneTitle.empty()) {
        text->setFontSize(40);
        int titleX = 50; int titleY = 50;
        text->drawText(renderer, sceneTitle, titleX + 3, titleY + 3, {0, 0, 0, 255});
        text->drawText(renderer, sceneTitle, titleX, titleY, {255, 215, 0, 255});
    }

    // Isi Cerita
    int startY = (int)(h * 0.40f); 
    int marginSide = 50;
    int maxWidth = w - (marginSide * 2);
    int lineHeight = 36;
    int paragraphGap = 20; 

    text->setFontSize(26);

    for (size_t i = 0; i <= currentLineIndex && i < lines.size(); ++i) {
        std::string rawLine = lines[i];
        std::vector<std::string> wrapped = wrapText(text, renderer, rawLine, maxWidth);

        for (const auto& lineStr : wrapped) {
            text->drawText(renderer, lineStr, marginSide + 2, startY + 2, {0,0,0,255});
            text->drawText(renderer, lineStr, marginSide + 1, startY + 1, {0,0,0,255});
            text->drawText(renderer, lineStr, marginSide, startY, {255, 255, 255, 255});
            startY += lineHeight;
        }
        startY += paragraphGap;
    }
    
    // --- HINT BUTTON & PROGRESS BAR (Visual Feedback) ---
    text->setFontSize(18);
    std::string hint;
    
    // Hitung progress bar timer (opsional, biar player tau kapan ganti)
    float progress = 0.0f;
    if (autoTimer < currentDuration) {
        progress = autoTimer / currentDuration;
    } else {
        progress = 1.0f; // Full
    }
    
    // Warna hint berubah kalau mau auto-next
    SDL_Color hintColor = {200, 200, 200, 255};

    if (currentLineIndex < lines.size() - 1) {
        hint = "AUTO PLAY >> [SPACE to Skip]"; 
    } else {
        hint = "NEXT SCENE >>"; 
        hintColor = {255, 255, 100, 255};
    }

    int hintW = text->measureWidth(renderer, hint);
    int hintX = w - hintW - 40;
    int hintY = h - 50;

    // Shadow Hint
    text->drawText(renderer, hint, hintX + 2, hintY + 2, {0,0,0,255});
    text->drawText(renderer, hint, hintX, hintY, hintColor);

    // (Opsional) Garis timer kecil di bawah teks hint
    if (currentLineIndex < lines.size() - 1) {
        SDL_Rect timerBar = {hintX, hintY + 25, (int)(hintW * progress), 2};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        SDL_RenderFillRect(renderer, &timerBar);
    }
}