#include "TextSystem.h"
#include <iostream>
#include <filesystem>
#include "Helper.h"

std::queue<TextDrawRequest> TextSystem::s_textQueue;
std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> TextSystem::s_fontMap;

void TextSystem::QueueText(const std::string& text,
    int x,
    int y,
    const std::string& fontName,
    int fontSize,
    int r,
    int g,
    int b,
    int a)
{
    TextDrawRequest req;
    req.text = text;
    req.x = x;
    req.y = y;
    req.fontName = fontName;
    req.fontSize = fontSize;
    req.r = r;
    req.g = g;
    req.b = b;
    req.a = a;
    s_textQueue.push(req);
}

TTF_Font* TextSystem::GetFont(const std::string& fontName, int fontSize)
{
    auto it1 = s_fontMap.find(fontName);
    if (it1 != s_fontMap.end())
    {
        auto it2 = it1->second.find(fontSize);
        if (it2 != it1->second.end())
        {
            return it2->second;
        }
    }

    std::string fontPath = "resources/fonts/" + fontName + ".ttf";
    if (!std::filesystem::exists(fontPath))
    {
        std::cout << "[TextSystem] WARNING: Font file not found: " << fontPath;
        return nullptr;
    }

    TTF_Font* newFont = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!newFont)
    {
        std::cout << "[TextSystem] ERROR: Failed to open font: " << fontPath
            << " TTF Error: " << TTF_GetError();
        return nullptr;
    }

    s_fontMap[fontName][fontSize] = newFont;
    return newFont;
}

void TextSystem::Flush(SDL_Renderer* renderer)
{
    while (!s_textQueue.empty())
    {
        TextDrawRequest req = s_textQueue.front();
        s_textQueue.pop();

        TTF_Font* font = GetFont(req.fontName, req.fontSize);
        if (!font)
        {
            continue;
        }

        SDL_Color colorForTTF = {
            static_cast<Uint8>(req.r),
            static_cast<Uint8>(req.g),
            static_cast<Uint8>(req.b),
            255
        };

        SDL_Surface* textSurface = nullptr;
        if (req.a == 255)
        {
            textSurface = TTF_RenderText_Solid(font, req.text.c_str(), colorForTTF);
        }
        else
        {
            textSurface = TTF_RenderText_Blended(font, req.text.c_str(), colorForTTF);
        }

        if (!textSurface)
        {
            std::cout << "[TextSystem] ERROR: TTF_RenderText_* failed: "
                << TTF_GetError();
            continue;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (!textTexture)
        {
            std::cout << "[TextSystem] ERROR: SDL_CreateTextureFromSurface failed: "
                << SDL_GetError();
            SDL_FreeSurface(textSurface);
            continue;
        }

        SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(textTexture, static_cast<Uint8>(req.a));

        SDL_FRect dstF;
        dstF.x = static_cast<float>(req.x);
        dstF.y = static_cast<float>(req.y);
        dstF.w = static_cast<float>(textSurface->w);
        dstF.h = static_cast<float>(textSurface->h);

        Helper::SDL_RenderCopyEx(-1, "text", renderer, textTexture, nullptr, &dstF,
            0.0f, nullptr, SDL_FLIP_NONE);

        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(textSurface);
    }
}
