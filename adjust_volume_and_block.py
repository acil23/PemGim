#!/usr/bin/env python3

# 1. Lower music volume to 5%
with open('src/Game.cpp', 'r') as f:
    content = f.read()

content = content.replace(
    '    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);',
    '    Mix_VolumeMusic(MIX_MAX_VOLUME / 20);  // 5% volume'
)

with open('src/Game.cpp', 'w') as f:
    f.write(content)

print("✓ Changed music volume to 5%")

# 2. Add block sound when blocking
with open('src/DuelScene.cpp', 'r') as f:
    content = f.read()

# Find the block message and add sound right before it
content = content.replace(
    '        std::cout << "[Blocked] Kamu berhasil memblokir serangan!\\n";',
    '''        std::cout << "[Blocked] Kamu berhasil memblokir serangan!\\n";
        if (gamePtr) {
            gamePtr->playSound("../assets/sounds/sword_block_sound.wav");
        }'''
)

with open('src/DuelScene.cpp', 'w') as f:
    f.write(content)

print("✓ Added block sound effect when blocking")
