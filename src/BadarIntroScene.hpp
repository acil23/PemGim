#pragma once
#include "Scene.hpp"
#include <string>
#include <vector>
#include "DialogueBox.hpp"     // <- penting

class BadarIntroScene : public Scene {
public:
    BadarIntroScene()
        : change(false), next(""),
          gamePtr(nullptr), npcFrame(0) {}

    void onEnter(Game* game) override;
    void onExit() override {}

    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return next.c_str(); }

private:
    bool change;
    std::string next;

    Game* gamePtr;
    DialogueBox dialog;   // <-- pastikan ini ADA
    Uint32 npcFrame;      // 0 atau 1
};
