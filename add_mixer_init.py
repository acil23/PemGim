#!/usr/bin/env python3

with open('src/Game.cpp', 'r') as f:
    content = f.read()

# Add SDL_mixer initialization after TTF_Init
init_code = '''    
    // Initialize SDL_mixer for audio
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
        std::cerr << "[SDL_mixer] Init failed: " << Mix_GetError() << "\\n";
        return false;
    }
    
    // Check what audio decoders are available
    int numDecoders = Mix_GetNumMusicDecoders();
    std::cout << "[SDL_mixer] Available music decoders: ";
    for (int i = 0; i < numDecoders; i++) {
        std::cout << Mix_GetMusicDecoder(i) << " ";
    }
    std::cout << "\\n";
    
    // Allocate mixing channels for sound effects
    Mix_AllocateChannels(16);
    
    // Set volume to 50%
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_Volume(-1, MIX_MAX_VOLUME / 2);
'''

# Insert after TTF_Init check
content = content.replace(
    '    if (TTF_Init() != 0) {\n        std::cerr << "[SDL_ttf] Init failed: " << TTF_GetError() << "\\n";\n        return false;\n    }',
    '''    if (TTF_Init() != 0) {
        std::cerr << "[SDL_ttf] Init failed: " << TTF_GetError() << "\\n";
        return false;
    }
''' + init_code
)

# Also need to add SDL_mixer include
if '#include <SDL_mixer.h>' not in content:
    content = content.replace(
        '#include <SDL_ttf.h>\n#include <iostream>',
        '#include <SDL_ttf.h>\n#include <SDL_mixer.h>\n#include <iostream>'
    )

# Add cleanup in shutdown
content = content.replace(
    '    TTF_Quit();\n    IMG_Quit();\n    SDL_Quit();\n}',
    '''    
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
}'''
)

with open('src/Game.cpp', 'w') as f:
    f.write(content)

print("✓ Added SDL_mixer initialization and cleanup to Game.cpp")
