#ifndef IMAGESYSTEM_H
#define IMAGESYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include "SDL2/SDL.h"

struct ImageDrawRequest
{
    bool isPixel = false;
    std::string image_name;
    float x = 0.0f;
    float y = 0.0f;
    float rotation_degrees = 0.0f;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float pivot_x = 0.0f;
    float pivot_y = 0.0f;

    int r = 255;
    int g = 255;
    int b = 255;
    int a = 255;

    int sorting_order = 0;
    bool isScene = false;

    int insertion_index = 0;
};

class ImageSystem
{
public:
    static void Init(SDL_Renderer* renderer);

    static void CreateDefaultParticleTexture(const std::string& name);

    static void DrawUI(const std::string& image_name, float x, float y);
    static void DrawUIEx(const std::string& image_name, float x, float y,
        float r, float g, float b, float a, float sorting_order);

    static void Draw(const std::string& image_name, float x, float y);
    static void DrawEx(const std::string& image_name, float x, float y,
        float rotation_degrees, float scale_x, float scale_y,
        float pivot_x, float pivot_y,
        float r, float g, float b, float a, float sorting_order);

    static void DrawPixel(float x, float y, float r, float g, float b, float a);

    static void FlushScene(SDL_Renderer* renderer);
    static void FlushUI(SDL_Renderer* renderer);
    static void FlushPixels(SDL_Renderer* renderer);

private:
    static SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::string& image_name);

    static inline std::vector<ImageDrawRequest> s_sceneRequests{};
    static inline std::vector<ImageDrawRequest> s_uiRequests{};
    static inline std::vector<ImageDrawRequest> s_pixelRequests{};

    static inline std::unordered_map<std::string, SDL_Texture*> s_textureMap{};

    static inline int s_insertionCounter = 0;
};

#endif
