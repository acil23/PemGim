#!/usr/bin/env python3

with open('src/Game.cpp', 'r') as f:
    content = f.read()

# Try 48000 Hz which is more common in modern systems and might match your files better
content = content.replace(
    'if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)',
    'if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) < 0)'
)

with open('src/Game.cpp', 'w') as f:
    f.write(content)

print("✓ Changed to 48000 Hz frequency and 4096 buffer size")
