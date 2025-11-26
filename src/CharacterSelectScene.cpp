#include "CharacterSelectScene.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath> // Untuk fungsi sin()
#include "TextRenderer.hpp"
#include "Game.hpp"

CharacterSelectScene::CharacterSelectScene() 
    : gamePtr(nullptr), change(false), nextScene(""), selectedIndex(1), // Default tengah
      chooseTexture(nullptr), bgTexture(nullptr), frameW(64), frameH(64),
      timeAccumulator(0.0f)
{
}

void CharacterSelectScene::onEnter(Game* game) {
    gamePtr = game;
    change = false;
    nextScene = "";
    selectedIndex = 1; // Mulai dari tengah (Hamzah biasanya center)
    timeAccumulator = 0.0f;

    // --- 1. Definisi Data Karakter (Merged Version) ---
    // Menggabungkan stat dan ID dari Versi 1 dengan format Versi 2
    candidates[0] = ChosenCharacterData{
        "Ali bin Abi Thalib",
        90,     // maxHP
        25,     // baseAttack
        0,      // frame index di spritesheet (choose.png)
        0       // characterID (untuk load player_attack.png nanti)
    };
    candidates[1] = ChosenCharacterData{
        "Hamzah bin Abdul Muthalib",
        120,    // maxHP (Tank)
        20,     // baseAttack
        1,      // frame index
        1       // characterID (untuk load player_attack2.png nanti)
    };
    candidates[2] = ChosenCharacterData{
        "Ubaidah bin Harits",
        100,    // maxHP (Balanced)
        18,     // baseAttack
        2,      // frame index
        2       // characterID (untuk load player_attack3.png nanti)
    };

    // --- 2. Load Assets ---
    // Load Background
    SDL_Surface* bgSurf = IMG_Load("../assets/images/choose_bg.jpg"); // Pastikan ekstensi .jpg atau .png sesuai filemu
    if (bgSurf) {
        bgTexture = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), bgSurf);
        SDL_FreeSurface(bgSurf);
    } else {
        // Fallback jika jpg tidak ada, coba png
        bgSurf = IMG_Load("../assets/images/choose_bg.png");
        if (bgSurf) {
            bgTexture = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), bgSurf);
            SDL_FreeSurface(bgSurf);
        } else {
            std::cerr << "[CharSelect] Gagal load choose_bg assets\n";
        }
    }

    // Load Characters Spritesheet (choose.png)
    SDL_Surface* surf = IMG_Load("../assets/images/choose.png");
    if (!surf) {
        std::cerr << "[CharSelect] Gagal load choose.png: " << IMG_GetError() << "\n";
        // Fallback size agar tidak crash saat render
        frameW = 64;
        frameH = 64;
    } else {
        chooseTexture = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), surf);
        // Asumsi: spritesheet berisi 3 karakter berjajar horizontal
        frameH = surf->h;
        frameW = surf->w / 3;
        SDL_FreeSurface(surf);
    }

    std::cout << "[CharacterSelectScene] Masuk pemilihan karakter.\n";
}

void CharacterSelectScene::onExit() {
    if (chooseTexture) { SDL_DestroyTexture(chooseTexture); chooseTexture = nullptr; }
    if (bgTexture) { SDL_DestroyTexture(bgTexture); bgTexture = nullptr; }
}

void CharacterSelectScene::handleEvent(const SDL_Event& e) {
    int winW = 1280, winH = 720;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);

    // --- MOUSE INPUT (Dari Versi 2 - Lebih Interaktif) ---
    if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        // LOGIKA POSISI (Harus sinkron dengan Render)
        // Hitung scale berdasarkan tinggi layar
        float targetCharHeight = winH * 0.35f;
        float scale = targetCharHeight / (float)frameH;
        if (scale < 1.0f) scale = 1.0f;

        int gap = (int)(winW * 0.05f); 
        int charW = (int)(frameW * scale);
        int charH = (int)(frameH * scale);
        int totalW = (3 * charW) + (2 * gap);
        int startX = (winW - totalW) / 2;
        int centerY = (winH / 2) - (int)(winH * 0.05f); 

        for (int i = 0; i < 3; ++i) {
            int x = startX + i * (charW + gap);
            int y = centerY - charH / 2; // Posisi dasar (tanpa bobbing)

            // Cek collision mouse
            if (mx >= x && mx <= x + charW && my >= y && my <= y + charH) {
                if (selectedIndex != i) {
                    selectedIndex = i; // Hover effect
                }
                
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    // KONFIRMASI PILIHAN
                    if (gamePtr) gamePtr->setChosenCharacter(candidates[selectedIndex]);
                    change = true;
                    // Lanjut ke Story Pre-Duel, bukan langsung Duel
                    nextScene = "pre_duel_story"; 
                    std::cout << "[CharacterSelectScene] Memilih: " << candidates[selectedIndex].displayName << "\n";
                }
            }
        }
    }

    // --- KEYBOARD INPUT ---
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            change = true; 
            nextScene = "menu";
            std::cout << "[CharacterSelectScene] Kembali ke menu.\n";
        }
        else if (key == SDLK_LEFT || key == SDLK_a) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (key == SDLK_RIGHT || key == SDLK_d) {
            if (selectedIndex < 2) selectedIndex++;
        }
        else if (key == SDLK_1) selectedIndex = 0;
        else if (key == SDLK_2) selectedIndex = 1;
        else if (key == SDLK_3) selectedIndex = 2;
        
        else if (key == SDLK_RETURN || key == SDLK_SPACE) {
            // KONFIRMASI PILIHAN
            if (gamePtr) gamePtr->setChosenCharacter(candidates[selectedIndex]);
            change = true;
            nextScene = "pre_duel_story";
            std::cout << "[CharacterSelectScene] Memilih: " << candidates[selectedIndex].displayName << "\n";
        }
    }
}

void CharacterSelectScene::update(float deltaTime) {
    // Update timer untuk animasi "bobbing" (naik turun halus)
    timeAccumulator += deltaTime * 5.0f; 
}

// Helper bikin kotak semi transparan
void CharacterSelectScene::drawPanel(SDL_Renderer* renderer, int x, int y, int w, int h) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // Hitam transparan
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    
    // Border emas
    SDL_SetRenderDrawColor(renderer, 200, 180, 50, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void CharacterSelectScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    int winW = 1280;
    int winH = 720;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);

    // 1. Gambar Background Fullscreen
    if (bgTexture) {
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
    } else {
        // Fallback background
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);
    }

    // 2. Judul Scene (Skala font menyesuaikan layar sedikit)
    int titleSize = (int)(winH * 0.06f); // Font judul ~6% tinggi layar
    text->setFontSize(titleSize);
    std::string title = "Pilih Pejuang Mubarazah";
    int tw = text->measureWidth(renderer, title);
    
    // Bayangan judul
    text->drawText(renderer, title, (winW - tw)/2 + 2, 42, {0,0,0,100}); 
    // Teks judul emas
    text->drawText(renderer, title, (winW - tw)/2, 40, {255, 223, 0, 255});

    // --- 3. Render Karakter (DYNAMIC SCALING & SILUET) ---
    if (chooseTexture) {
        // Hitung scale berdasarkan tinggi layar (35%)
        float targetCharHeight = winH * 0.35f;
        float scale = targetCharHeight / (float)frameH;
        if (scale < 1.0f) scale = 1.0f;

        int gap = (int)(winW * 0.05f); 
        int charW = (int)(frameW * scale);
        int charH = (int)(frameH * scale);
        int totalW = (3 * charW) + (2 * gap);
        int startX = (winW - totalW) / 2;
        int centerY = (winH / 2) - (int)(winH * 0.05f); 

        for (int i = 0; i < 3; ++i) {
            bool isSelected = (i == selectedIndex);
            
            int x = startX + i * (charW + gap);
            
            // Animasi bobbing jika terpilih
            int bounceY = 0;
            if (isSelected) {
                bounceY = static_cast<int>(sin(timeAccumulator) * 5.0f);
            }
            
            int y = centerY - (charH / 2) + bounceY;

            // Setup Source Rect
            SDL_Rect src = { candidates[i].spriteIndex * frameW, 0, frameW, frameH };

            // Setup Dest Rect (Efek Pop-up saat dipilih)
            float finalScale = isSelected ? (scale * 1.15f) : scale;
            SDL_Rect dst;
            dst.w = (int)(frameW * finalScale);
            dst.h = (int)(frameH * finalScale);
            // Koreksi posisi center
            dst.x = x - (dst.w - charW) / 2;
            dst.y = y - (dst.h - charH); 

            // --- RENDER LOGIKA SILUET ---
            if (isSelected) {
                // Warna Asli
                SDL_SetTextureColorMod(chooseTexture, 255, 255, 255);
                SDL_SetTextureAlphaMod(chooseTexture, 255);
                
                // Shadow sederhana di bawah kaki
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
                SDL_Rect shadowRect = {dst.x + 10, dst.y + dst.h - 8, dst.w - 20, 8};
                SDL_RenderFillRect(renderer, &shadowRect);
            } else {
                // Siluet Hitam
                SDL_SetTextureColorMod(chooseTexture, 0, 0, 0); 
                SDL_SetTextureAlphaMod(chooseTexture, 180); 
            }

            SDL_RenderCopy(renderer, chooseTexture, &src, &dst);
        }
        
        // Reset Texture agar tidak mempengaruhi render lain
        SDL_SetTextureColorMod(chooseTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(chooseTexture, 255);
    }

    // --- 4. Render Info Box (RESPONSIVE) ---
    const auto& ch = candidates[selectedIndex];
    
    // Panel lebar menyesuaikan layar (60% lebar layar)
    int panelW = (int)(winW * 0.6f); 
    int panelH = (int)(winH * 0.22f); 
    int panelX = (winW - panelW) / 2;
    int panelY = winH - panelH - 30; // 30px dari bawah

    // Gambar panel background
    drawPanel(renderer, panelX, panelY, panelW, panelH);

    // --- Layout Teks ---
    
    // 1. Nama Karakter (Tengah Atas)
    text->setFontSize((int)(panelH * 0.18f)); 
    int nameW = text->measureWidth(renderer, ch.displayName);
    text->drawText(renderer, ch.displayName, panelX + (panelW - nameW)/2, panelY + 15, {255, 255, 255, 255});

    // Garis pemisah
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    int lineY = panelY + (int)(panelH * 0.35f);
    SDL_RenderDrawLine(renderer, panelX + 50, lineY, panelX + panelW - 50, lineY);

    // 2. Stats (Kiri & Kanan sejajar di bawah garis)
    text->setFontSize((int)(panelH * 0.14f));
    
    std::string hpTxt = "Health: " + std::to_string(ch.maxHP);
    std::string atkTxt = "Attack: " + std::to_string(ch.baseAttack);
    
    int hpW = text->measureWidth(renderer, hpTxt);
    int atkW = text->measureWidth(renderer, atkTxt);

    int statsY = lineY + 15;
    int offsetCenter = panelW / 4; 
    
    text->drawText(renderer, hpTxt, panelX + (panelW/2) - offsetCenter - (hpW/2), statsY, {100, 255, 100, 255});
    text->drawText(renderer, atkTxt, panelX + (panelW/2) + offsetCenter - (atkW/2), statsY, {255, 100, 100, 255});

    // 3. Deskripsi
    std::string desc = "";
    if (selectedIndex == 0) desc = "Cepat & Tangkas. Memiliki serangan beruntun mematikan.";
    else if (selectedIndex == 1) desc = "Kuat & Kokoh. Sangat tangguh menahan serangan musuh.";
    else desc = "Seimbang & Taktis. Ahli strategi pertarungan jarak menengah.";

    text->setFontSize((int)(panelH * 0.12f)); 
    int descW = text->measureWidth(renderer, desc);
    int descY = statsY + 35;
    text->drawText(renderer, desc, panelX + (panelW - descW)/2, descY, {220, 220, 220, 255});

    // Hint Controls (Footer)
    text->setFontSize(16);
    text->drawText(renderer, "[ARROWS] Pilih   [ENTER] Konfirmasi   [ESC] Kembali", 20, winH - 30, {255, 255, 255, 150});
}