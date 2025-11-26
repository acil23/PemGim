#!/usr/bin/env python3

for filename in ['src/DuelScene.cpp', 'src/StoryScene.cpp']:
    with open(filename, 'r') as f:
        content = f.read()
    
    content = content.replace('.ogg"', '.wav"')
    
    with open(filename, 'w') as f:
        f.write(content)
    
    print(f"✓ Changed .ogg to .wav in {filename}")
