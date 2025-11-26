#!/bin/bash
cd /home/waz/Game/assets/sounds

echo "Converting OGG files to WAV format..."

for f in *.ogg; do
    echo "Converting $f..."
    ffmpeg -i "$f" -acodec pcm_s16le -ar 44100 "${f%.ogg}.wav" -y 2>&1 | tail -3
done

echo ""
echo "WAV files created:"
ls -lh *.wav
