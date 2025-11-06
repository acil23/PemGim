#pragma once
#include "Scene.hpp"
#include <string>

class MenuScene : public Scene {
public:
    MenuScene() : change(false), next("") {}

    void onEnter(Game* /*game*/) override {}
    void onExit() override {}

    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return next.c_str(); }

private:
    bool change;
    std::string next;
};
