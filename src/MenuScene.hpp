#pragma once
#include "Scene.hpp"
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>

// Enum untuk membedakan kita sedang di halaman menu mana
enum class MenuState {
    MAIN_MENU,
    WAR_SELECT,
    ENCYCLOPEDIA,
    SETTINGS
};

struct MenuItem {
    std::string label;
    std::string actionKey; 
};

class MenuScene : public Scene {
public:
    MenuScene();

    void onEnter(Game* game) override;
    void onExit() override;

    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return next.c_str(); }

private:
    bool change;
    std::string next;
    Game* gamePtr = nullptr;

    // --- State ---
    MenuState currentMenuState;
    int selectedItem = 0;

    // --- Data Menu (Daftar Tombol per Halaman) ---
    std::vector<MenuItem> mainMenuItems;
    std::vector<MenuItem> warMenuItems;
    std::vector<MenuItem> encyMenuItems;
    std::vector<MenuItem> settingsItems;

    // Pointer helper untuk menunjuk menu mana yang sedang aktif
    std::vector<MenuItem>* currentItemsPtr = nullptr;

    // --- Resources ---
    SDL_Texture* bgTexture = nullptr;
    SDL_Texture* buttonIdleTex = nullptr;
    SDL_Texture* buttonHoverTex = nullptr;
    int btnW = 0;
    int btnH = 0;
    Mix_Music* menuMusic = nullptr;

    SDL_Texture* loadTexture(const std::string& path);
    
    // Helper untuk mereset pilihan saat ganti halaman
    void switchState(MenuState newState);
    void executeSelectedItem();
};