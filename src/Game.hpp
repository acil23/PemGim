#pragma once
#include <SDL.h>
#include <string>
#include "SceneManager.hpp"
#include "TextRenderer.hpp"

struct ChosenCharacterData {
    std::string displayName;
    int maxHP;
    int baseAttack;
    // nanti bisa tambah sprite rect / texture ID dsb
    int spriteIndex; // 0 = kiri (putih), 1 = tengah (biru), 2 = kanan (merah)
};


class Game {
public:
    Game();
    ~Game();

    bool init(const std::string& title, int width, int height, bool fullscreen);
    bool isFullscreen = false;
    
    void setChosenCharacter(const ChosenCharacterData& data) {
        currentCharacter = data;
    }
    const ChosenCharacterData& getChosenCharacter() const {
        return currentCharacter;
    }

    void toggleFullscreen();
    void getWindowSize(int& w, int& h);

    void run();
    void shutdown();
    

    // Getter untuk resource global yang dibutuhkan scene
    SDL_Texture* getNpcTexture() { return npcTexture; }
    TextRenderer* getTextRenderer() { return &textRenderer; }

    SDL_Renderer* getRenderer() { return renderer; }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    SceneManager sceneManager;
    TextRenderer textRenderer;

    SDL_Texture* npcTexture = nullptr;
    ChosenCharacterData currentCharacter;

    void processEvents();
    void update(float deltaTime);
    void render();

    Uint64 lastTicks = 0;
};
