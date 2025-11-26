#!/usr/bin/env python3

with open('src/Game.cpp', 'r') as f:
    content = f.read()

# Match the audio frequency to the file's native frequency
content = content.replace(
    'if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) < 0)',
    'if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)'
)

with open('src/Game.cpp', 'w') as f:
    f.write(content)

print("✓ Changed SDL_mixer frequency to 44100 Hz to match audio files")
