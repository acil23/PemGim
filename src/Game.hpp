#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <map>
#include "SceneManager.hpp"
#include "TextRenderer.hpp"

struct ChosenCharacterData {
    std::string displayName;
    int maxHP;
    int baseAttack;
    // nanti bisa tambah sprite rect / texture ID dsb
    int spriteIndex; // 0 = kiri (putih), 1 = tengah (biru), 2 = kanan (merah)
    int characterID; // 0, 1, or 2 for which character asset to use
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
    
    // Audio functions
    void playMusic(const std::string& path, int loops = -1);
    void stopMusic();
    void playSound(const std::string& path);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    SceneManager sceneManager;
    TextRenderer textRenderer;

    SDL_Texture* npcTexture = nullptr;
    ChosenCharacterData currentCharacter;
    
    // Audio resources
    Mix_Music* currentMusic = nullptr;
    std::map<std::string, Mix_Chunk*> soundEffects;

    void processEvents();
    void update(float deltaTime);
    void render();

    Uint64 lastTicks = 0;
};
