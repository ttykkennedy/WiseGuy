#ifndef TEXTSYSTEM_H
#define TEXTSYSTEM_H

#include <string>
#include <queue>
#include <unordered_map>
#include "SDL_ttf/SDL_ttf.h"
#include "SDL2/SDL.h"

struct TextDrawRequest
{
    std::string text;
    int x;
    int y;
    std::string fontName;
    int fontSize;
    int r, g, b, a;
};

class TextSystem
{
public:
    static void QueueText(const std::string& text,
        int x,
        int y,
        const std::string& fontName,
        int fontSize,
        int r,
        int g,
        int b,
        int a);

    static void Flush(SDL_Renderer* renderer);

private:
    static TTF_Font* GetFont(const std::string& fontName, int fontSize);

    static std::queue<TextDrawRequest> s_textQueue;

    static std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> s_fontMap;
};

#endif
