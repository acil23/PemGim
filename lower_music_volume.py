#!/usr/bin/env python3

with open('src/Game.cpp', 'r') as f:
    content = f.read()

# Change music volume from 50% to 25%
content = content.replace(
    '    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);',
    '    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);'
)

with open('src/Game.cpp', 'w') as f:
    f.write(content)

print("✓ Changed music volume to 25%")
