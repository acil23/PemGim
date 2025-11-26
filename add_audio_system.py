#!/usr/bin/env python3
import re

# 1. Add SDL_mixer to Game.hpp
with open('src/Game.hpp', 'r') as f:
    hpp_content = f.read()

# Add includes
hpp_content = hpp_content.replace(
    '#include <SDL.h>\n#include <string>',
    '#include <SDL.h>\n#include <SDL_mixer.h>\n#include <string>\n#include <map>'
)

# Add audio function declarations before "private:"
hpp_content = hpp_content.replace(
    '    SDL_Renderer* getRenderer() { return renderer; }\n\nprivate:',
    '''    SDL_Renderer* getRenderer() { return renderer; }
    
    // Audio functions
    void playMusic(const std::string& path, int loops = -1);
    void stopMusic();
    void playSound(const std::string& path);

private:'''
)

# Add audio member variables
hpp_content = hpp_content.replace(
    '    SDL_Texture* npcTexture = nullptr;\n    ChosenCharacterData currentCharacter;\n\n    void processEvents();',
    '''    SDL_Texture* npcTexture = nullptr;
    ChosenCharacterData currentCharacter;
    
    // Audio resources
    Mix_Music* currentMusic = nullptr;
    std::map<std::string, Mix_Chunk*> soundEffects;

    void processEvents();'''
)

with open('src/Game.hpp', 'w') as f:
    f.write(hpp_content)

print("✓ Updated Game.hpp with audio declarations")

# 2. Add audio implementation to Game.cpp at the end
with open('src/Game.cpp', 'r') as f:
    cpp_lines = f.readlines()

# Find the last closing brace
last_brace_idx = -1
for i in range(len(cpp_lines) - 1, -1, -1):
    if cpp_lines[i].strip() == '}':
        last_brace_idx = i
        break

# Insert audio functions before the last function
audio_impl = '''
void Game::playMusic(const std::string& path, int loops) {
    // Stop current music
    if (currentMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    
    std::cout << "[Audio] Loading music: " << path << "\\n";
    
    // Load new music
    currentMusic = Mix_LoadMUS(path.c_str());
    if (!currentMusic) {
        std::cerr << "[Audio] Failed to load music: " << path << " - " << Mix_GetError() << "\\n";
        return;
    }
    
    // Play music (loops: -1 = infinite, 0 = once, 1+ = that many times)
    if (Mix_PlayMusic(currentMusic, loops) == -1) {
        std::cerr << "[Audio] Failed to play music: " << Mix_GetError() << "\\n";
    } else {
        std::cout << "[Audio] Playing music successfully\\n";
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
        std::cout << "[Audio] Loading sound: " << path << "\\n";
        // Load sound effect
        Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
        if (!chunk) {
            std::cerr << "[Audio] Failed to load sound: " << path << " - " << Mix_GetError() << "\\n";
            return;
        }
        soundEffects[path] = chunk;
    }
    
    // Play sound effect on any available channel
    if (Mix_PlayChannel(-1, soundEffects[path], 0) == -1) {
        std::cerr << "[Audio] Failed to play sound: " << Mix_GetError() << "\\n";
    }
}
'''

cpp_lines.append(audio_impl)

with open('src/Game.cpp', 'w') as f:
    f.writelines(cpp_lines)

print("✓ Added audio implementation to Game.cpp")
