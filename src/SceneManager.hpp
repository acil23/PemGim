#pragma once
#include <memory>
#include <string>
#include "Scene.hpp"

class Game;

class SceneManager {
public:
    SceneManager(Game* g) : game(g) {}
    // ganti scene aktif berdasarkan "nama"
    void changeScene(const std::string& name);

    // akses scene aktif
    Scene* getCurrentScene() { return currentScene.get(); }

private:
    std::unique_ptr<Scene> currentScene;
    Game* game;

    // factory internal
    std::unique_ptr<Scene> createSceneByName(const std::string& name);
};
