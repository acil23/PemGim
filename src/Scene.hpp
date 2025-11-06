#pragma once
#include <SDL.h>

class TextRenderer; // forward declare biar ga circular include
class Game; // forward

class Scene {
public:
    virtual ~Scene() {}

    virtual void onEnter(Game* game) {}
    virtual void onExit() {}

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual void update(float deltaTime) = 0;

    // render sekarang dapat renderer + textRenderer
    virtual void render(SDL_Renderer* renderer,
                        TextRenderer* text) = 0;

    virtual bool wantsSceneChange() const { return false; }
    virtual const char* nextSceneName() const { return nullptr; }
};
