#!/usr/bin/env python3

with open('src/DuelScene.cpp', 'r') as f:
    lines = f.readlines()

new_lines = []
for i, line in enumerate(lines):
    new_lines.append(line)
    
    # Add sound to startPlayerAttack
    if line.strip() == 'hasHitThisSwing = false;' and i > 270 and i < 280:
        new_lines.append('    \n')
        new_lines.append('    // Play sword sound\n')
        new_lines.append('    if (gamePtr) {\n')
        new_lines.append('        gamePtr->playSound("assets/sounds/sword_sound.ogg");\n')
        new_lines.append('    }\n')
    
    # Add sound to startPlayerJump  
    if line.strip() == 'jumpCurrentFrame = 0;' and i > 370 and i < 385:
        new_lines.append('        \n')
        new_lines.append('        // Play jump sound\n')
        new_lines.append('        if (gamePtr) {\n')
        new_lines.append('            gamePtr->playSound("assets/sounds/jump.ogg");\n')
        new_lines.append('        }\n')
    
    # Add sound to block detection (we'll add it in checkEnemyHitAndDamagePlayer where blocking happens)

with open('src/DuelScene.cpp', 'w') as f:
    f.writelines(new_lines)

print("✓ Added sword and jump sound effects")

# Now add block sound to checkEnemyHitAndDamagePlayer
with open('src/DuelScene.cpp', 'r') as f:
    content = f.read()

# Find the blocking section and add sound
content = content.replace(
    "if (blocking) {\n        // player is blocking, no damage\n        return;\n    }",
    '''if (blocking) {
        // player is blocking, no damage
        if (gamePtr) {
            gamePtr->playSound("assets/sounds/sword_block_sound.ogg");
        }
        return;
    }'''
)

with open('src/DuelScene.cpp', 'w') as f:
    f.write(content)

print("✓ Added block sound effect")
