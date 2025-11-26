#!/usr/bin/env python3

# 1. Stop music in MenuScene onEnter
with open('src/MenuScene.cpp', 'r') as f:
    content = f.read()

content = content.replace(
    'void MenuScene::onEnter(Game* game) {\n    gamePtr = game;',
    '''void MenuScene::onEnter(Game* game) {
    gamePtr = game;
    
    // Stop any music when entering menu
    if (gamePtr) {
        gamePtr->stopMusic();
    }'''
)

with open('src/MenuScene.cpp', 'w') as f:
    f.write(content)

print("✓ Added stopMusic() to MenuScene::onEnter()")

# 2. Play story music again after duel in DuelScene onExit
with open('src/DuelScene.cpp', 'r') as f:
    lines = f.readlines()

# Find onExit function and add music playback
new_lines = []
in_onexit = False
added = False

for i, line in enumerate(lines):
    new_lines.append(line)
    
    if 'void DuelScene::onExit()' in line:
        in_onexit = True
    
    if in_onexit and not added and line.strip() == '{':
        new_lines.append('    // Resume story music after duel\n')
        new_lines.append('    if (gamePtr) {\n')
        new_lines.append('        gamePtr->playMusic("../assets/sounds/story_song.ogg", -1);\n')
        new_lines.append('    }\n')
        new_lines.append('    \n')
        added = True
        in_onexit = False

with open('src/DuelScene.cpp', 'w') as f:
    f.writelines(new_lines)

print("✓ Added story music playback to DuelScene::onExit()")
