#include "MenuScene.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "TextRenderer.hpp"
#include "Game.hpp"

MenuScene::MenuScene() : change(false), next("") {
    // --- 1. SETUP ITEM MENU UTAMA ---
    mainMenuItems = {
        {"Mulai Perang", "GOTO_WAR"},
        {"Ensiklopedia Sirah", "GOTO_ENCY"},
        {"Pengaturan", "GOTO_SETTINGS"},
        {"Keluar", "EXIT_GAME"}
    };

    // --- 2. SETUP SUB-MENU PERANG ---
    warMenuItems = {
        {"Perang Badar (2 H)", "badar_intro"}, // Langsung ke gameplay
        {"Perang Uhud (3 H)", "LOCKED"},       // Contoh fitur belum jadi
        {"Perang Khandaq (5 H)", "LOCKED"},
        {"Kembali", "BACK_TO_MAIN"}
    };

    // --- 3. SETUP SUB-MENU ENSIKLOPEDIA ---
    encyMenuItems = {
        {"Sirah: Badar", "ENCY_BADAR"},       // Nanti buat scene baru utk bacaan
        {"Sirah: Uhud", "LOCKED"},
        {"Tokoh-Tokoh", "LOCKED"},
        {"Kembali", "BACK_TO_MAIN"}
    };

    // --- 4. SETUP SETTINGS ---
    settingsItems = {
        {"Layar Penuh (Toggle)", "TOGGLE_FULLSCREEN"},
        {"Suara (On/Off)", "TOGGLE_SOUND"}, // Placeholder
        {"Kembali", "BACK_TO_MAIN"}
    };

    currentMenuState = MenuState::MAIN_MENU;
    currentItemsPtr = &mainMenuItems;
}

SDL_Texture* MenuScene::loadTexture(const std::string& path) {
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (!surf) {
        std::cerr << "[Menu] Gagal load: " << path << " -> " << IMG_GetError() << "\n";
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), surf);
    if (path.find("btn_idle") != std::string::npos && surf) {
        btnW = surf->w;
        btnH = surf->h;
    }
    SDL_FreeSurface(surf);
    return tex;
}

void MenuScene::switchState(MenuState newState) {
    currentMenuState = newState;
    selectedItem = 0; // Reset kursor ke paling atas tiap ganti halaman

    // Update pointer agar render & event loop otomatis pakai list yang baru
    switch (newState) {
        case MenuState::MAIN_MENU:    currentItemsPtr = &mainMenuItems; break;
        case MenuState::WAR_SELECT:   currentItemsPtr = &warMenuItems; break;
        case MenuState::ENCYCLOPEDIA: currentItemsPtr = &encyMenuItems; break;
        case MenuState::SETTINGS:     currentItemsPtr = &settingsItems; break;
    }
}

void MenuScene::onEnter(Game* game) {
    gamePtr = game;
    change = false;
    next = "";
    
    // Load aset (Pastikan path benar)
    bgTexture = loadTexture("../assets/images/menu/background.png");
    buttonIdleTex = loadTexture("../assets/images/menu/btn_idle.png");
    buttonHoverTex = loadTexture("../assets/images/menu/btn_hover.png");
    
    // Mulai dari menu utama
    switchState(MenuState::MAIN_MENU);
}

void MenuScene::onExit() {
    if (bgTexture) SDL_DestroyTexture(bgTexture);
    if (buttonIdleTex) SDL_DestroyTexture(buttonIdleTex);
    if (buttonHoverTex) SDL_DestroyTexture(buttonHoverTex);
    bgTexture = nullptr;
    buttonIdleTex = nullptr;
    buttonHoverTex = nullptr;
}

void MenuScene::executeSelectedItem() {
    // Safety check
    if (!currentItemsPtr || currentItemsPtr->empty()) return;

    // Ambil aksi dari item yang sedang dipilih
    std::string action = (*currentItemsPtr)[selectedItem].actionKey;
    std::cout << "[MenuScene] Eksekusi aksi: " << action << "\n";

    // --- LOGIKA NAVIGASI MENU (Sama seperti sebelumnya) ---
    if (action == "GOTO_WAR") {
        switchState(MenuState::WAR_SELECT);
    }
    else if (action == "GOTO_ENCY") {
        switchState(MenuState::ENCYCLOPEDIA);
    }
    else if (action == "GOTO_SETTINGS") {
        switchState(MenuState::SETTINGS);
    }
    else if (action == "BACK_TO_MAIN") {
        switchState(MenuState::MAIN_MENU);
    }
    // --- LOGIKA GAME ---
    else if (action == "EXIT_GAME") {
        change = true; next = "";
    }
    else if (action == "LOCKED") {
        std::cout << "[Menu] Fitur terkunci!\n";
    }
    else if (action == "TOGGLE_FULLSCREEN") {
        if (gamePtr) gamePtr->toggleFullscreen();
    }
    else if (action == "TOGGLE_SOUND") {
        std::cout << "[Menu] Toggle Sound (Not Implemented)\n";
    }
    // --- PINDAH KE SCENE LAIN ---
    else {
        change = true;
        next = action;
    }
}

void MenuScene::handleEvent(const SDL_Event& e) {
    // --- INPUT MOUSE ---
    if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        int winW = 1280, winH = 720; // Default fallback
        if (gamePtr) gamePtr->getWindowSize(winW, winH);

        // Kita harus menghitung ulang posisi tombol (Logic harus sama persis dengan render)
        // Agar mouse mendeteksi koordinat yang tepat
        int startY = 160;
        int gap = 15;
        
        // Loop cek semua tombol di halaman ini
        for (size_t i = 0; i < currentItemsPtr->size(); ++i) {
            // Hitung area tombol ke-i
            SDL_Rect btnRect;
            btnRect.w = btnW;
            btnRect.h = btnH;
            btnRect.x = (winW - btnW) / 2;
            btnRect.y = startY + (int)i * (btnH + gap);

            // Cek tabrakan: Apakah Mouse ada di dalam btnRect?
            bool inside = (mx >= btnRect.x && mx <= btnRect.x + btnRect.w &&
                           my >= btnRect.y && my <= btnRect.y + btnRect.h);

            if (inside) {
                // 1. Efek Hover: Kalau mouse gerak di atas tombol, ubah seleksi
                if (selectedItem != (int)i) {
                    selectedItem = (int)i;
                    // Opsional: playSound("hover.wav");
                }

                // 2. Klik Kiri: Eksekusi
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    executeSelectedItem();
                }
            }
        }
    }

    // --- INPUT KEYBOARD ---
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            if (currentMenuState == MenuState::MAIN_MENU) {
                change = true; next = ""; 
            } else {
                switchState(MenuState::MAIN_MENU); 
            }
        }
        else if (key == SDLK_UP || key == SDLK_w) {
            selectedItem--;
            if (selectedItem < 0) selectedItem = (int)currentItemsPtr->size() - 1;
        }
        else if (key == SDLK_DOWN || key == SDLK_s) {
            selectedItem++;
            if (selectedItem >= (int)currentItemsPtr->size()) selectedItem = 0;
        }
        else if (key == SDLK_RETURN || key == SDLK_SPACE || key == SDLK_KP_ENTER) {
            // Panggil fungsi helper yang sudah kita buat tadi
            executeSelectedItem();
        }
    }
}

void MenuScene::update(float /*deltaTime*/) {
    // Animasi background atau partikel debu bisa ditambahkan di sini
}

void MenuScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    int winW = 1280, winH = 720;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);

    // 1. Gambar Background
    if (bgTexture) {
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
    } else {
        SDL_SetRenderDrawColor(renderer, 30, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    // 2. Tentukan Judul Berdasarkan Halaman
    std::string mainTitle = "MUBARAZAH";
    std::string subTitle = "Chronicles of the Prophet's Wars";
    SDL_Color titleColor = {255, 215, 0, 255}; // Emas

    if (currentMenuState == MenuState::WAR_SELECT) {
        mainTitle = "PILIH PERANG";
        subTitle = "Pilih pertempuran yang ingin dimainkan";
    } else if (currentMenuState == MenuState::ENCYCLOPEDIA) {
        mainTitle = "ENSIKLOPEDIA";
        subTitle = "Pelajari sejarah dan tokoh";
    } else if (currentMenuState == MenuState::SETTINGS) {
        mainTitle = "PENGATURAN";
        subTitle = "Sesuaikan pengalaman bermain";
    }

    // Render Judul
    text->setFontSize(48);
    int t1w = text->measureWidth(renderer, mainTitle);
    text->drawText(renderer, mainTitle, (winW - t1w)/2, 40, titleColor);

    text->setFontSize(20);
    int t2w = text->measureWidth(renderer, subTitle);
    text->drawText(renderer, subTitle, (winW - t2w)/2, 95, {200, 200, 200, 255});

    // 3. Render Tombol (Dinamis sesuai halaman aktif)
    text->setFontSize(22);
    int startY = 160;
    int gap = 15;

    // Kita loop item yang sedang aktif (ditunjuk oleh currentItemsPtr)
    for (size_t i = 0; i < currentItemsPtr->size(); ++i) {
        const auto& item = (*currentItemsPtr)[i];
        bool isSelected = ((int)i == selectedItem);

        SDL_Texture* tex = isSelected ? buttonHoverTex : buttonIdleTex;

        SDL_Rect dst;
        dst.w = btnW; 
        dst.h = btnH;
        dst.x = (winW - btnW) / 2;
        dst.y = startY + (int)i * (btnH + gap);

        // Gambar tombol
        if (tex) SDL_RenderCopy(renderer, tex, nullptr, &dst);

        // Siapkan label teks
        std::string labelToDraw = item.label;

        // KHUSUS SETTINGS: Update label real-time
        if (currentMenuState == MenuState::SETTINGS) {
            if (item.actionKey == "TOGGLE_FULLSCREEN") {
                bool fs = (gamePtr && gamePtr->isFullscreen);
                labelToDraw += fs ? " [ON]" : " [OFF]";
            }
        }

        // Gambar Teks Tombol
        int tw = text->measureWidth(renderer, labelToDraw);
        int th = text->measureHeight(renderer, labelToDraw);
        
        SDL_Color txtColor = isSelected ? SDL_Color{255,255,255,255} : SDL_Color{180,180,160,255};
        
        // Locked item warnanya abu gelap
        if (item.actionKey == "LOCKED") txtColor = {100, 100, 100, 255};

        text->drawText(renderer, labelToDraw, 
                       dst.x + (dst.w - tw)/2, 
                       dst.y + (dst.h - th)/2 - 2, 
                       txtColor);
    }
    
    // Footer hint
    text->setFontSize(14);
    if (currentMenuState != MenuState::MAIN_MENU) {
        text->drawText(renderer, "[ESC] Kembali", 20, winH - 30, {150,150,150,255});
    } else {
        text->drawText(renderer, "v0.5-alpha", 20, winH - 30, {100,100,100,255});
    }
}