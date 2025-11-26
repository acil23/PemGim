#!/usr/bin/env python3

# Add audio to DuelScene.cpp
with open('src/DuelScene.cpp', 'r') as f:
    lines = f.readlines()

# Find the line with "gamePtr = game;" and insert audio code after it
new_lines = []
for i, line in enumerate(lines):
    new_lines.append(line)
    if 'gamePtr = game;' in line and i < 20:  # Make sure it's in onEnter
        new_lines.append('    \n')
        new_lines.append('    // Play fight music\n')
        new_lines.append('    if (gamePtr) {\n')
        new_lines.append('        gamePtr->stopMusic();\n')
        new_lines.append('        gamePtr->playMusic("assets/sounds/fight_song.ogg", -1);\n')
        new_lines.append('    }\n')

with open('src/DuelScene.cpp', 'w') as f:
    f.writelines(new_lines)

print("✓ Added fight music to DuelScene.cpp")

# Add audio to StoryScene.cpp
with open('src/StoryScene.cpp', 'r') as f:
    content = f.read()

# Add music playback in onEnter
content = content.replace(
    '        }\n    }\n}\n\nvoid StoryScene::onExit()',
    '''        }
    }
    
    // Play story music only if not already playing
    if (gamePtr && !Mix_PlayingMusic()) {
        gamePtr->playMusic("assets/sounds/story_song.ogg", -1);
    }
}

void StoryScene::onExit() {
    // Stop music if transitioning to duel
    if (gamePtr && next == "duel") {
        gamePtr->stopMusic();
    }'''
)

with open('src/StoryScene.cpp', 'w') as f:
    f.write(content)

print("✓ Added story music to StoryScene.cpp")
