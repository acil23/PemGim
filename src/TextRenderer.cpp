#include "TextRenderer.hpp"
#include <iostream>

TextRenderer::TextRenderer()
    : font(nullptr), currentSize(0) {}

TextRenderer::~TextRenderer() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

bool TextRenderer::loadFont(const std::string& path, int ptSize) {
    // simpan info biar bisa resize nanti
    currentPath = path;
    currentSize = ptSize;

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    font = TTF_OpenFont(path.c_str(), ptSize);
    if (!font) {
        std::cerr << "[TextRenderer] Failed to load font "
                  << path << ": " << TTF_GetError() << "\n";
        return false;
    }

    return true;
}

void TextRenderer::setFontSize(int ptSize) {
    if (currentPath.empty()) return;
    if (ptSize == currentSize) return;

    currentSize = ptSize;

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    font = TTF_OpenFont(currentPath.c_str(), currentSize);
    if (!font) {
        std::cerr << "[TextRenderer] Failed to resize font: "
                  << TTF_GetError() << "\n";
    }
}

void TextRenderer::drawText(SDL_Renderer* renderer,
                            const std::string& text,
                            int x, int y,
                            SDL_Color color)
{
    if (!font) {
        // amanin aja biar ga crash kalau lupa loadFont
        std::cerr << "[TextRenderer] drawText called but font == nullptr\n";
        return;
    }

    // render text jadi surface
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) {
        std::cerr << "[TextRenderer] TTF_RenderUTF8_Blended error: "
                  << TTF_GetError() << "\n";
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) {
        std::cerr << "[TextRenderer] SDL_CreateTextureFromSurface error: "
                  << SDL_GetError() << "\n";
        SDL_FreeSurface(surf);
        return;
    }

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = surf->w;
    dst.h = surf->h;

    SDL_RenderCopy(renderer, tex, nullptr, &dst);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

int TextRenderer::measureWidth(SDL_Renderer* /*renderer*/, const std::string& textStr) {
    if (!font) return 0;

    int w = 0;
    int h = 0;
    if (TTF_SizeUTF8(font, textStr.c_str(), &w, &h) != 0) {
        // kalau gagal, balikkan 0 biar aman
        return 0;
    }
    return w;
}

int TextRenderer::measureHeight(SDL_Renderer* /*renderer*/, const std::string& textStr) {
    if (!font) return 0;

    int w = 0;
    int h = 0;
    if (TTF_SizeUTF8(font, textStr.c_str(), &w, &h) != 0) {
        return 0;
    }
    return h;
}
