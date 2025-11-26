#!/usr/bin/env python3

for filename in ['src/DuelScene.cpp', 'src/StoryScene.cpp']:
    with open(filename, 'r') as f:
        content = f.read()
    
    content = content.replace('assets/sounds/', '../assets/sounds/')
    
    with open(filename, 'w') as f:
        f.write(content)
    
    print(f"✓ Fixed paths in {filename}")
