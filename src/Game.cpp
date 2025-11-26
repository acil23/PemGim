#include "Game.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
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
    
    // Initialize SDL_mixer for audio
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
        std::cerr << "[SDL_mixer] Init failed: " << Mix_GetError() << "\n";
        return false;
    }
    
    // Check what audio decoders are available
    int numDecoders = Mix_GetNumMusicDecoders();
    std::cout << "[SDL_mixer] Available music decoders: ";
    for (int i = 0; i < numDecoders; i++) {
        std::cout << Mix_GetMusicDecoder(i) << " ";
    }
    std::cout << "\n";
    
    // Allocate mixing channels for sound effects
    Mix_AllocateChannels(16);
    
    // Set volume to 50%
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_Volume(-1, MIX_MAX_VOLUME / 2);


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

    
    // Clean up audio resources
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    
    for (auto& pair : soundEffects) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    soundEffects.clear();
    
    Mix_CloseAudio();
    
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


void Game::playMusic(const std::string& path, int loops) {
    // Stop current music
    if (currentMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    
    std::cout << "[Audio] Loading music: " << path << "\n";
    
    // Load new music
    currentMusic = Mix_LoadMUS(path.c_str());
    if (!currentMusic) {
        std::cerr << "[Audio] Failed to load music: " << path << " - " << Mix_GetError() << "\n";
        return;
    }
    
    // Play music (loops: -1 = infinite, 0 = once, 1+ = that many times)
    if (Mix_PlayMusic(currentMusic, loops) == -1) {
        std::cerr << "[Audio] Failed to play music: " << Mix_GetError() << "\n";
    } else {
        std::cout << "[Audio] Playing music successfully\n";
    }
}

void Game::stopMusic() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
}

void Game::playSound(const std::string& path) {
    // Check if sound is already loaded
    if (soundEffects.find(path) == soundEffects.end()) {
        std::cout << "[Audio] Loading sound: " << path << "\n";
        // Load sound effect
        Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
        if (!chunk) {
            std::cerr << "[Audio] Failed to load sound: " << path << " - " << Mix_GetError() << "\n";
            return;
        }
        soundEffects[path] = chunk;
    }
    
    // Play sound effect on any available channel
    if (Mix_PlayChannel(-1, soundEffects[path], 0) == -1) {
        std::cerr << "[Audio] Failed to play sound: " << Mix_GetError() << "\n";
    }
}
