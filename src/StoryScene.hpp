#pragma once
#include "Scene.hpp"
#include <string>
#include <vector>

class StoryScene : public Scene {
public:
    StoryScene();
    
    void setup(const std::string& bgPath, 
               const std::string& title, 
               const std::vector<std::string>& textLines, 
               const std::string& nextSceneID);

    void onEnter(Game* game) override;
    void onExit() override;
    void handleEvent(const SDL_Event& e) override;
    
    // UPDATE PENTING: Kita akan pakai deltaTime di sini
    void update(float deltaTime) override;
    
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return next.c_str(); }

private:
    Game* gamePtr = nullptr;
    bool change;
    std::string next;

    std::string backgroundPath;
    SDL_Texture* bgTexture = nullptr;
    
    std::string sceneTitle;
    std::vector<std::string> lines;
    size_t currentLineIndex = 0; 

    // --- VARIABEL AUTO NEXT ---
    float autoTimer = 0.0f;        // Penghitung waktu berjalan
    float currentDuration = 0.0f;  // Target waktu untuk baris saat ini
    
    // Helper hitung waktu baca
    float calculateReadingTime(const std::string& text);

    std::vector<std::string> wrapText(TextRenderer* text, SDL_Renderer* r, 
                                      const std::string& str, int maxWidth);
};