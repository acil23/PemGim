#!/usr/bin/env python3

with open('src/Game.cpp', 'r') as f:
    content = f.read()

# Replace the Mix_OpenAudio line with better quality settings
content = content.replace(
    'if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)',
    'if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)'
)

with open('src/Game.cpp', 'w') as f:
    f.write(content)

print("✓ Updated SDL_mixer to use MIX_DEFAULT_FORMAT and larger buffer (2048)")
