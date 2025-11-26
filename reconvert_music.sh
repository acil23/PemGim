#!/bin/bash
cd /home/waz/Game/assets/sounds

echo "Re-converting music files with optimal settings for SDL_mixer..."

# Convert only the music files (not SFX) with high-quality settings
echo "Converting story_song.ogg..."
ffmpeg -i story_song.ogg -acodec pcm_s16le -ar 44100 -ac 2 -sample_fmt s16 story_song_new.wav -y 2>&1 | tail -3

echo "Converting fight_song.ogg..."
ffmpeg -i fight_song.ogg -acodec pcm_s16le -ar 44100 -ac 2 -sample_fmt s16 fight_song_new.wav -y 2>&1 | tail -3

# Replace old files
mv story_song_new.wav story_song.wav
mv fight_song_new.wav fight_song.wav

echo ""
echo "Music files re-converted:"
ls -lh story_song.wav fight_song.wav
