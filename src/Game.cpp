#include "Game.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <SDL_image.h>
#include <SDL_ttf.h>

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      running(false),
      isFullscreen(false),
      sceneManager(this)
{}


Game::~Game() {
    shutdown();
}

bool Game::init(const std::string& title, int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "[SDL] Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    // init SDL_image untuk PNG
    int imgFlags = IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) == 0) {
        std::cerr << "[SDL_image] Init failed: " << IMG_GetError() << "\n";
        return false;
    }

    if (TTF_Init() != 0) {
        std::cerr << "[SDL_ttf] Init failed: " << TTF_GetError() << "\n";
        return false;
    }

    Uint32 flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags
    );
    if (!window) {
        std::cerr << "[SDL] CreateWindow failed: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "[SDL] CreateRenderer failed: " << SDL_GetError() << "\n";
        return false;
    }

    // load font utama
    // NOTE: pastikan path ini bener sesuai project kamu
    if (!textRenderer.loadFont("../assets/fonts/DejaVuSans.ttf", 18)) {
        std::cerr << "[Game] Failed to load font. Pastikan file .ttf ada.\n";
        // kita lanjut aja tapi teks ga akan muncul
    }

    // load npc texture
    {
        SDL_Surface* surf = IMG_Load("../assets/images/NPCs.png");
        if (!surf) {
            std::cerr << "[Game] Failed to load NPC sprite: " << IMG_GetError() << "\n";
        } else {
            npcTexture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if (!npcTexture) {
                std::cerr << "[Game] Failed to create texture from NPC surface: "
                          << SDL_GetError() << "\n";
            }
        }
    }

    running = true;

    sceneManager.changeScene("menu");
    lastTicks = SDL_GetTicks64();
    return true;
}

void Game::run() {
    while (running) {
        Uint64 currentTicks = SDL_GetTicks64();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_F11) {
                toggleFullscreen();
            }
        }

        if (sceneManager.getCurrentScene()) {
            sceneManager.getCurrentScene()->handleEvent(e);
        }
    }
}

void Game::update(float deltaTime) {
    auto* scene = sceneManager.getCurrentScene();
    if (!scene) {
        running = false;
        return;
    }

    scene->update(deltaTime);

    if (scene->wantsSceneChange()) {
        const char* nextName = scene->nextSceneName();
        
        // --- PERBAIKAN DI SINI ---
        // Cek: Apakah pointer valid DAN huruf pertamanya bukan null terminator?
        if (nextName && nextName[0] != '\0') {
            // Kalau namanya ada isinya (misal "duel"), ganti scene
            sceneManager.changeScene(nextName);
        } else {
            // Kalau string kosong (""), artinya Quit Game
            std::cout << "[Game] Sinyal keluar diterima. Shutting down...\n";
            running = false;
        }
    }
}

void Game::render() {
    auto* scene = sceneManager.getCurrentScene();
    if (!scene) return;

    scene->render(renderer, &textRenderer);

    SDL_RenderPresent(renderer);
}

void Game::shutdown() {
    // hancurkan renderer/window dulu sebelum quit subsystems
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::toggleFullscreen() {
    if (!window) return;

    isFullscreen = !isFullscreen;
    if (isFullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        
        // UBAH DI SINI JUGA: Samakan dengan main.cpp (1280, 720)
        SDL_SetWindowSize(window, 1280, 720);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

void Game::getWindowSize(int& w, int& h) {
    if (!window) { w = 0; h = 0; return; }
    SDL_GetWindowSize(window, &w, &h);
}

