#pragma once
#include "Scene.hpp"
#include <string>
#include <array>
#include "Game.hpp" // for ChosenCharacterData

class CharacterSelectScene : public Scene {
public:
    CharacterSelectScene()
        : gamePtr(nullptr),
          change(false),
          nextScene(""),
          selectedIndex(0),
          chooseTexture(nullptr),
          frameW(64),
          frameH(64)
    {}

    void onEnter(Game* game) override;
    void onExit() override {}

    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return nextScene.c_str(); }

private:
    Game* gamePtr;
    bool change;
    std::string nextScene;

    int selectedIndex; // 0 = Ali?, 1 = Hamzah?, 2 = Ubaidah?

    SDL_Texture* chooseTexture;
    int frameW;
    int frameH;

    // Data kandidat karakter
    std::array<ChosenCharacterData,3> candidates;
};
