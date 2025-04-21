#include "ImageSystem.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include "SDL_image/SDL_image.h"
#include "Helper.h"
#include "Camera.h"

static SDL_Renderer* s_renderer = nullptr;

void ImageSystem::Init(SDL_Renderer* renderer)
{
    s_renderer = renderer;
}

void ImageSystem::CreateDefaultParticleTexture(const std::string& name)
{
    if (s_textureMap.find(name) != s_textureMap.end())
        return;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!surface)
    {
        std::cout << "[ImageSystem] Failed to create 8x8 surface: " << SDL_GetError() << std::endl;
        return;
    }

    Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    SDL_FillRect(surface, nullptr, white_color);

    if (!s_renderer)
    {
        std::cout << "[ImageSystem] s_renderer is null; cannot create texture.\n";
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(s_renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        std::cout << "[ImageSystem] SDL_CreateTextureFromSurface failed: "
                  << SDL_GetError() << std::endl;
        return;
    }

    s_textureMap[name] = texture;
}

static bool compareBySortThenInsertion(const ImageDrawRequest& a,
    const ImageDrawRequest& b)
{
    if (a.sorting_order < b.sorting_order) return true;
    if (a.sorting_order > b.sorting_order) return false;
    return (a.insertion_index < b.insertion_index);
}

void ImageSystem::DrawUI(const std::string& image_name, float x, float y)
{
    ImageDrawRequest req;
    req.isPixel = false;
    req.image_name = image_name;
    req.x = x;
    req.y = y;
    req.r = 255;
    req.g = 255;
    req.b = 255;
    req.a = 255;
    req.sorting_order = 0;
    req.isScene = false;
    req.insertion_index = s_insertionCounter++;
    s_uiRequests.push_back(req);
}

void ImageSystem::DrawUIEx(const std::string& image_name, float x, float y,
    float r, float g, float b, float a, float sorting_order)
{
    ImageDrawRequest req;
    req.isPixel = false;
    req.image_name = image_name;
    req.x = x;
    req.y = y;
    req.r = static_cast<int>(r);
    req.g = static_cast<int>(g);
    req.b = static_cast<int>(b);
    req.a = static_cast<int>(a);
    req.sorting_order = static_cast<int>(sorting_order);
    req.isScene = false;
    req.insertion_index = s_insertionCounter++;
    s_uiRequests.push_back(req);
}

void ImageSystem::Draw(const std::string& image_name, float x, float y)
{
    ImageDrawRequest req;
    req.isPixel = false;
    req.image_name = image_name;
    req.x = x;
    req.y = y;
    req.pivot_x = 0.5f;
    req.pivot_y = 0.5f;
    req.scale_x = 1.0f;
    req.scale_y = 1.0f;
    req.rotation_degrees = 0.0f;
    req.r = 255;
    req.g = 255;
    req.b = 255;
    req.a = 255;
    req.sorting_order = 0;
    req.isScene = true;
    req.insertion_index = s_insertionCounter++;
    s_sceneRequests.push_back(req);
}

void ImageSystem::DrawEx(const std::string& image_name, float x, float y,
    float rotation_degrees, float scale_x, float scale_y,
    float pivot_x, float pivot_y,
    float r, float g, float b, float a, float sorting_order)
{
    ImageDrawRequest req;
    req.isPixel = false;
    req.image_name = image_name;
    req.x = x;
    req.y = y;
    req.rotation_degrees = rotation_degrees;
    req.scale_x = scale_x;
    req.scale_y = scale_y;
    req.pivot_x = pivot_x;
    req.pivot_y = pivot_y;
    req.r = static_cast<int>(r);
    req.g = static_cast<int>(g);
    req.b = static_cast<int>(b);
    req.a = static_cast<int>(a);
    req.sorting_order = static_cast<int>(sorting_order);
    req.isScene = true;
    req.insertion_index = s_insertionCounter++;
    s_sceneRequests.push_back(req);
}

void ImageSystem::DrawPixel(float x, float y, float r, float g, float b, float a)
{
    ImageDrawRequest req;
    req.isPixel = true;
    req.x = x;
    req.y = y;
    req.r = static_cast<int>(r);
    req.g = static_cast<int>(g);
    req.b = static_cast<int>(b);
    req.a = static_cast<int>(a);
    req.insertion_index = s_insertionCounter++;
    s_pixelRequests.push_back(req);
}

SDL_Texture* ImageSystem::LoadTexture(SDL_Renderer* renderer, const std::string& image_name)
{
    auto it = s_textureMap.find(image_name);
    if (it != s_textureMap.end())
    {
        return it->second;
    }

    static std::string gTryExtensions[] = { ".png", ".jpg", ".jpeg", ".bmp" };
    std::string basePath = "resources/images/";
    SDL_Texture* foundTex = nullptr;

    for (auto& ext : gTryExtensions)
    {
        std::string attempt = basePath + image_name + ext;
        if (std::filesystem::exists(attempt))
        {
            foundTex = IMG_LoadTexture(renderer, attempt.c_str());
            if (!foundTex)
            {
                std::cout << "[ImageSystem] Failed to load "
                    << attempt << " : " << IMG_GetError() << std::endl;
            }
            else
            {
                break;
            }
        }
    }

    if (!foundTex)
    {
        std::cout << "[ImageSystem] Could not locate an image: "
            << image_name << std::endl;
        return nullptr;
    }

    s_textureMap[image_name] = foundTex;
    return foundTex;
}

void ImageSystem::FlushScene(SDL_Renderer* renderer)
{
    if (s_sceneRequests.empty()) return;

    float camX = Camera::GetPositionX();
    float camY = Camera::GetPositionY();
    float zoom = Camera::GetZoom();

    SDL_RenderSetScale(renderer, zoom, zoom);

    std::stable_sort(s_sceneRequests.begin(), s_sceneRequests.end(), compareBySortThenInsertion);

    const float px_per_meter = 100.0f;
    glm::ivec2 camDim = Camera::GetCameraDimensions();

    for (auto& req : s_sceneRequests)
    {
        if (req.isPixel) continue;

        SDL_Texture* tex = LoadTexture(renderer, req.image_name);
        if (!tex) continue;

        float w = 0.0f, h = 0.0f;
        Helper::SDL_QueryTexture(tex, &w, &h);

        float finalW = w * req.scale_x;
        float finalH = h * req.scale_y;

        float pivot_px = req.pivot_x * finalW;
        float pivot_py = req.pivot_y * finalH;

        float finalX = ((req.x - camX) * px_per_meter)
                       + (camDim.x * 0.5f * (1.0f / zoom))
                       - pivot_px;

        float finalY = ((req.y - camY) * px_per_meter)
                       + (camDim.y * 0.5f * (1.0f / zoom))
                       - pivot_py;

        SDL_FRect dstRect;
        dstRect.x = finalX;
        dstRect.y = finalY;
        dstRect.w = finalW;
        dstRect.h = finalH;

        SDL_SetTextureColorMod(tex, req.r, req.g, req.b);
        SDL_SetTextureAlphaMod(tex, req.a);

        SDL_FPoint center;
        center.x = pivot_px;
        center.y = pivot_py;

        Helper::SDL_RenderCopyEx(
            -1,
            "image",
            renderer,
            tex,
            nullptr,
            &dstRect,
            req.rotation_degrees,
            &center,
            SDL_FLIP_NONE
        );

        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);
    }

    SDL_RenderSetScale(renderer, 1.0f, 1.0f);

    s_sceneRequests.clear();
}
void ImageSystem::FlushUI(SDL_Renderer* renderer)
{
    if (s_uiRequests.empty()) return;

    std::stable_sort(s_uiRequests.begin(), s_uiRequests.end(), compareBySortThenInsertion);

    for (auto& req : s_uiRequests)
    {
        if (req.isPixel) continue;

        SDL_Texture* tex = LoadTexture(renderer, req.image_name);
        if (!tex) continue;

        float w = 0.0f, h = 0.0f;
        Helper::SDL_QueryTexture(tex, &w, &h);
        int texW = static_cast<int>(w);
        int texH = static_cast<int>(h);

        float finalW = static_cast<float>(texW);
        float finalH = static_cast<float>(texH);

        float pivot_px = req.pivot_x * finalW;
        float pivot_py = req.pivot_y * finalH;

        float dst_x = req.x - pivot_px;
        float dst_y = req.y - pivot_py;

        SDL_FRect dstRect;
        dstRect.x = dst_x;
        dstRect.y = dst_y;
        dstRect.w = finalW;
        dstRect.h = finalH;

        SDL_SetTextureColorMod(tex, req.r, req.g, req.b);
        SDL_SetTextureAlphaMod(tex, req.a);

        SDL_FPoint center;
        center.x = pivot_px;
        center.y = pivot_py;

        Helper::SDL_RenderCopyEx(
            -1,
            "ui",
            renderer,
            tex,
            nullptr,
            &dstRect,
            req.rotation_degrees,
            &center,
            SDL_FLIP_NONE
        );

        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);
    }

    s_uiRequests.clear();
}

void ImageSystem::FlushPixels(SDL_Renderer* renderer)
{
    if (s_pixelRequests.empty()) return;

    std::stable_sort(s_pixelRequests.begin(), s_pixelRequests.end(), compareBySortThenInsertion);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (auto& req : s_pixelRequests)
    {
        if (!req.isPixel) continue;

        SDL_SetRenderDrawColor(renderer, req.r, req.g, req.b, req.a);
        SDL_RenderDrawPoint(
            renderer,
            static_cast<int>(req.x),
            static_cast<int>(req.y)
        );
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    s_pixelRequests.clear();
}
