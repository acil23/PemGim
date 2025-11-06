#pragma once
#include <SDL.h>
#include <string>
#include <vector>

class TextRenderer;
class Game;

class DialogueBox {
public:
    DialogueBox();

    // set daftar kalimat untuk dialog ini
    void setLines(const std::vector<std::string>& newLines);

    // panggil saat player tekan next (SPACE/ENTER)
    // return true kalau masih ada lanjut, false kalau sudah habis
    bool next();

    // render dialog box
    void render(SDL_Renderer* renderer,
                TextRenderer* text,
                SDL_Texture* npcTexture,
                int screenW,
                int screenH,
                Uint32 frameIndex /*0 atau 1*/);

    bool isFinished() const { return finished; }
  
private:
    std::vector<std::string> lines;
    size_t current = 0;
    bool finished = false;

    std::vector<std::string> wrapTextLines(TextRenderer* text,
                                           const std::string& raw,
                                           int maxWidth,
                                           SDL_Renderer* renderer) const;
};
