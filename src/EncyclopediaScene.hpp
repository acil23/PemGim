#pragma once
#include "Scene.hpp"
#include <string>
#include <vector>
#include <SDL_mixer.h>

struct EncyPage {
    std::string title;
    std::string content;
    std::string mediaPath; // Path ke gambar/folder frame video
};

class EncyclopediaScene : public Scene {
public:
    EncyclopediaScene();

    void onEnter(Game* game) override;
    void onExit() override;
    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return next.c_str(); }

private:
    Game* gamePtr = nullptr;
    bool change;
    std::string next;

    // --- DATA ---
    std::vector<EncyPage> pages;
    int currentPage = 0;

    // --- RESOURCES ---
    SDL_Texture* bookSpriteSheet = nullptr; // book.jpg
    SDL_Texture* currentMediaTex = nullptr; // Video frame
    SDL_Texture* arrowTex = nullptr;        // Tombol panah (optional, atau pakai text)
    Mix_Music* bgMusic = nullptr;

    // --- ANIMASI BUKU ---
    // book.jpg punya 4 kolom x 2 baris (8 frame)
    int bookFrameW = 0;
    int bookFrameH = 0;
    
    bool isTurning = false;    // Sedang animasi ganti halaman?
    float animTimer = 0.0f;
    int animFrame = 0;         // Frame 0-7
    int turnDirection = 1;     // 1 = Next, -1 = Prev

    // --- ANIMASI VIDEO (Simulasi) ---
    float videoTimer = 0.0f;
    int videoFrameIndex = 0;   // Misal untuk looping frame gambar

    // Helper
    void loadPageMedia(int pageIndex);
    void startPageTurn(int direction); // 1 for next, -1 for prev
    
    // Text Wrapper khusus buku (warna hitam/coklat)
    void drawBookText(SDL_Renderer* renderer, TextRenderer* text, int startX, int startY);
    std::vector<std::string> wrapText(TextRenderer* text, SDL_Renderer* r, const std::string& str, int maxWidth);
};