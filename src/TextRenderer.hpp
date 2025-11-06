#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    // load font dari path, size px
    bool loadFont(const std::string& path, int ptSize);

    // gambar text ke layar
    // x, y = posisi pojok kiri atas
    void drawText(SDL_Renderer* renderer,
                  const std::string& text,
                  int x, int y,
                  SDL_Color color = {255,255,255,255});

    // optional: ganti font di runtime
    void setFontSize(int ptSize);

    int measureWidth(SDL_Renderer* renderer, const std::string& textStr);
    int measureHeight(SDL_Renderer* renderer, const std::string& textStr);


private:
    TTF_Font* font;
    int currentSize;
    std::string currentPath;
};
