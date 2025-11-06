#include "DialogueBox.hpp"
#include "TextRenderer.hpp"
#include <SDL.h>
#include <sstream>

std::vector<std::string> DialogueBox::wrapTextLines(
    TextRenderer* text,
    const std::string& raw,
    int maxWidth,
    SDL_Renderer* renderer
) const {
    std::vector<std::string> result;

    std::istringstream iss(raw);
    std::string word;
    std::string currentLine;

    auto lineWidth = [&](const std::string& s) {
        return text->measureWidth(renderer, s);
    };

    while (iss >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        if (lineWidth(testLine) > maxWidth) {
            if (!currentLine.empty()) {
                result.push_back(currentLine);
            }
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }

    if (!currentLine.empty()) {
        result.push_back(currentLine);
    }

    return result;
}


DialogueBox::DialogueBox() {}

void DialogueBox::setLines(const std::vector<std::string>& newLines) {
    lines = newLines;
    current = 0;
    finished = (lines.empty());
}

bool DialogueBox::next() {
    if (finished) return false;
    current++;
    if (current >= lines.size()) {
        finished = true;
        return false;
    }
    return true;
}

void DialogueBox::render(SDL_Renderer* renderer,
                         TextRenderer* text,
                         SDL_Texture* npcTexture,
                         int screenW,
                         int screenH,
                         Uint32 frameIndex)
{
    if (finished || lines.empty()) {
        return;
    }

    const int pad = 20;
    SDL_Rect boxRect;
    boxRect.x = pad;
    boxRect.y = screenH - 160;    // sedikit dinaikkan biar lega
    boxRect.w = screenW - pad*2;
    boxRect.h = 140;

    // kotak semi-transparan
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &boxRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);
    SDL_RenderDrawRect(renderer, &boxRect);

    // --- NPC frame ---
    const int FRAME_W = 51.5;
    const int FRAME_H = 64;

    SDL_Rect srcRect;
    srcRect.x = frameIndex * FRAME_W;
    srcRect.y = 0;
    srcRect.w = FRAME_W;
    srcRect.h = FRAME_H;

    SDL_Rect dstRect;
    dstRect.x = boxRect.x + 16;
    dstRect.y = boxRect.y + 16;
    dstRect.w = FRAME_W * 2;
    dstRect.h = FRAME_H * 2;

    if (npcTexture) {
        SDL_RenderCopy(renderer, npcTexture, &srcRect, &dstRect);
    }

    // --- TEXT WRAP AREA ---
    // teks akan mulai setelah NPC
    int textX = dstRect.x + dstRect.w + 16;
    int textY = boxRect.y + 20;

    // berapa lebar area teks yg boleh dipakai?
    int textMaxWidth = boxRect.x + boxRect.w - 20 - textX; // margin kanan -20

    const std::string& currentLine = lines[current];

    // bungkus jadi beberapa baris
    std::vector<std::string> wrappedLines =
        wrapTextLines(text, currentLine, textMaxWidth, renderer);

    // gambar tiap baris turun 22px
    const int lineSpacing = 22;
    for (size_t i = 0; i < wrappedLines.size(); ++i) {
        text->drawText(renderer,
            wrappedLines[i],
            textX,
            textY + static_cast<int>(i) * lineSpacing,
            SDL_Color{255,255,255,255});
    }

    // hint next di pojok kanan bawah box
    text->drawText(renderer,
        "[SPACE/ENTER] lanjut...",
        boxRect.x + boxRect.w - 210,
        boxRect.y + boxRect.h - 24,
        SDL_Color{200,200,200,200});
}

