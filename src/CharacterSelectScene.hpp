#pragma once
#include "Scene.hpp"
#include <string>
#include <array>
#include <vector> // Perlu vector untuk layouting
#include "Game.hpp" 

class CharacterSelectScene : public Scene {
public:
    CharacterSelectScene();

    void onEnter(Game* game) override;
    void onExit() override;

    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return nextScene.c_str(); }

private:
    Game* gamePtr = nullptr;
    bool change;
    std::string nextScene;

    int selectedIndex; 

    // Resources
    SDL_Texture* chooseTexture = nullptr; // Spritesheet karakter
    SDL_Texture* bgTexture = nullptr;     // Background gurun
    
    // Dimensi Frame per karakter di spritesheet
    int frameW;
    int frameH;

    // Data kandidat
    std::array<ChosenCharacterData,3> candidates;

    // Animasi
    float timeAccumulator = 0.0f; // Untuk animasi naik turun (bobbing)

    // Helper untuk UI
    void drawPanel(SDL_Renderer* renderer, int x, int y, int w, int h);
};