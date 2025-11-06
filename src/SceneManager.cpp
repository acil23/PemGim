#include "SceneManager.hpp"
#include "MenuScene.hpp"
#include "BadarIntroScene.hpp"
#include "DuelScene.hpp"
#include "EndingScene.hpp"
#include <iostream>
#include "CharacterSelectScene.hpp"

std::unique_ptr<Scene> SceneManager::createSceneByName(const std::string& name) {
    if (name == "menu") {
        return std::make_unique<MenuScene>();
    } else if (name == "badar_intro") {
        return std::make_unique<BadarIntroScene>();
    } else if (name == "char_select") {
        return std::make_unique<CharacterSelectScene>();  // NEW
    } else if (name == "duel") {
        return std::make_unique<DuelScene>();
    } else if (name == "ending") {
        return std::make_unique<EndingScene>();
    } else {
        std::cerr << "[SceneManager] Unknown scene: " << name << "\n";
        return nullptr;
    }
}

void SceneManager::changeScene(const std::string& name) {
    if (currentScene) {
        currentScene->onExit();
    }
    currentScene = createSceneByName(name);
    if (currentScene) {
        currentScene->onEnter(game);
        std::cout << "[SceneManager] Switched to scene: " << name << "\n";
    } else {
        std::cerr << "[SceneManager] Failed to switch to scene: " << name << "\n";
    }
}
