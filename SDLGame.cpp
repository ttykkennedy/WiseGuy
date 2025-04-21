#include "SDLGame.h"
#include <SDL2/SDL.h>
#include <SDL_image/SDL_image.h>
#include <SDL_ttf/SDL_ttf.h>
#include <iostream>
#include <filesystem>
#include <cstdio>
#include <algorithm>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "Input.h"
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "Helper.h"
#include "Scene.h"
#include "ComponentDB.h"
#include "Actor.h"
#include "TextSystem.h"
#include "AudioHelper.h"
#include "AudioSystem.h"
#include "ImageSystem.h"
#include "ParticleSystem.h"
#include "Rigidbody.h"
#include "WwiseAudioSystem.h"

extern b2World* g_physicsWorld;
static bool         s_wantsToLoadScene = false;
static std::string  s_nextSceneName = "";
static std::string  g_currentSceneName = "basic";
static void Scene_Load(const std::string& sceneName);
static std::string Scene_GetCurrent();
static void Scene_DontDestroy(Actor* actor);
static void Scene_LoadNextSceneIfRequested(Scene& currentScene);
static void ReportError(const std::string& actor_name, const luabridge::LuaException& e);
static void InjectPropertiesIntoLuaRef(std::shared_ptr<luabridge::LuaRef>& instancePtr,
    const rapidjson::Value& compData);
static bool LoadActorTemplateJson(const std::string& templateID, rapidjson::Document& outDoc);
static Actor* Actor_Instantiate(const std::string& actor_template_name);
static void Actor_Destroy(Actor* actor);
static void Application_Quit();
static void Application_Sleep(int milliseconds);
static int  Application_GetFrame();
static void Application_OpenURL(const std::string& url);
static bool BuildSceneActorsAndComponents(Scene& scene, const std::string& scenePath);

constexpr AkGameObjectID LISTENER_ID = 1;
constexpr AkGameObjectID PLAYER_ID = 2;

int runSDLGame()
{
    int windowWidth = 640;
    int windowHeight = 360;
    int clearColorR = 0;
    int clearColorG = 0;
    int clearColorB = 0;
    {
        std::string renderingConfigPath = "resources/rendering.config";
        if (std::filesystem::exists(renderingConfigPath))
        {
            FILE* fp2 = nullptr;
#ifdef _WIN32
            fopen_s(&fp2, renderingConfigPath.c_str(), "rb");
#else
            fp2 = fopen(renderingConfigPath.c_str(), "rb");
#endif
            if (fp2)
            {
                char buf[65536];
                rapidjson::FileReadStream frs(fp2, buf, sizeof(buf));
                rapidjson::Document doc2;
                doc2.ParseStream(frs);
                fclose(fp2);

                if (!doc2.HasParseError() && doc2.IsObject())
                {
                    if (doc2.HasMember("x_resolution") && doc2["x_resolution"].IsInt())
                    {
                        windowWidth = doc2["x_resolution"].GetInt();
                    }
                    if (doc2.HasMember("y_resolution") && doc2["y_resolution"].IsInt())
                    {
                        windowHeight = doc2["y_resolution"].GetInt();
                    }
                    if (doc2.HasMember("clear_color_r") && doc2["clear_color_r"].IsInt())
                    {
                        clearColorR = doc2["clear_color_r"].GetInt();
                    }
                    if (doc2.HasMember("clear_color_g") && doc2["clear_color_g"].IsInt())
                    {
                        clearColorG = doc2["clear_color_g"].GetInt();
                    }
                    if (doc2.HasMember("clear_color_b") && doc2["clear_color_b"].IsInt())
                    {
                        clearColorB = doc2["clear_color_b"].GetInt();
                    }
                }
            }
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError();
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cout << "IMG_Init Error: " << IMG_GetError();
        SDL_Quit();
        return 1;
    }
    if (TTF_Init() != 0)
    {
        std::cout << "TTF_Init Error: " << TTF_GetError();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    if (AudioHelper::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    {
        std::cout << "Mix_OpenAudio error: " << SDL_GetError();
    }
    AudioHelper::Mix_AllocateChannels(50);

    SDL_Window* window = Helper::SDL_CreateWindow(
        "Lua Test #6",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );
    if (!window)
    {
        std::cout << "Error creating window: " << SDL_GetError();
        AudioHelper::Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = Helper::SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer)
    {
        std::cout << "Error creating renderer: " << SDL_GetError();
        SDL_DestroyWindow(window);
        AudioHelper::Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    ImageSystem::Init(renderer);
    ComponentDB::InitLua();
    Input::Init();

    if (!WwiseAudioSystem::Init())
    {
        std::cout << "[Wwise] Could not initialize. Continuing without Wwise.\n";
    }

    WwiseAudioSystem::RegisterGameObj(LISTENER_ID, "Listener");
    WwiseAudioSystem::RegisterGameObj(PLAYER_ID, "Player");
    WwiseAudioSystem::SetListener(LISTENER_ID);

    using namespace luabridge;
    getGlobalNamespace(ComponentDB::lua_state)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass()
        .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetID", &Actor::GetID)
        .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("GetComponents", &Actor::GetComponents)
        .addFunction("AddComponent", &Actor::AddComponent)
        .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass()
        .beginNamespace("Actor")
        .addFunction("Find",
            +[](const std::string& name) -> Actor*
            {
                if (!Scene::g_currentScene) return nullptr;
                for (auto* a : Scene::g_currentScene->actors)
                {
                    if (!a || a->destroyed) continue;
                    if (a->name == name) return a;
                }
                for (auto* a : Scene::g_currentScene->pendingActorsToAdd)
                {
                    if (!a || a->destroyed) continue;
                    if (a->name == name) return a;
                }
                return nullptr;
            })
        .addFunction("FindAll",
            +[](const std::string& name) -> LuaRef
            {
                LuaRef arr = luabridge::newTable(ComponentDB::lua_state);
                if (!Scene::g_currentScene)
                    return arr;
                int index = 1;
                for (auto* a : Scene::g_currentScene->actors)
                {
                    if (!a || a->destroyed) continue;
                    if (a->name == name)
                    {
                        arr[index++] = a;
                    }
                }
                for (auto* a : Scene::g_currentScene->pendingActorsToAdd)
                {
                    if (!a || a->destroyed) continue;
                    if (a->name == name)
                    {
                        arr[index++] = a;
                    }
                }
                return arr;
            })
        .addFunction("Instantiate", &Actor_Instantiate)
        .addFunction("Destroy", &Actor_Destroy)
        .endNamespace()
        .beginNamespace("Application")
        .addFunction("Quit", &Application_Quit)
        .addFunction("Sleep", &Application_Sleep)
        .addFunction("GetFrame", &Application_GetFrame)
        .addFunction("OpenURL", &Application_OpenURL)
        .endNamespace()
        .beginNamespace("Input")
        .addFunction("GetKey",
            +[](const std::string& key) {
                return Input::GetKey(key);
            })
        .addFunction("GetKeyDown",
            +[](const std::string& key) {
                return Input::GetKeyDown(key);
            })
        .addFunction("GetKeyUp",
            +[](const std::string& key) {
                return Input::GetKeyUp(key);
            })
        .addFunction("GetMousePosition",
            +[] {
                return Input::GetMousePosition();
            })
        .addFunction("GetMouseButton", &Input::GetMouseButton)
        .addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
        .addFunction("HideCursor", &Input::HideCursor)
        .addFunction("ShowCursor", &Input::ShowCursor)
        .endNamespace()
        .beginNamespace("Text")
        .addFunction("Draw",
            +[](const std::string& str_content,
                float x,
                float y,
                const std::string& font_name,
                float font_size,
                float r,
                float g,
                float b,
                float a)
            {
                TextSystem::QueueText(
                    str_content,
                    static_cast<int>(x),
                    static_cast<int>(y),
                    font_name,
                    static_cast<int>(font_size),
                    static_cast<int>(r),
                    static_cast<int>(g),
                    static_cast<int>(b),
                    static_cast<int>(a)
                );
            })
        .endNamespace()
        .beginNamespace("Audio")
        .addFunction("Play",
            +[](int channel, const std::string& clipName, bool doesLoop)
            {
                AudioSystem::Play(channel, clipName, doesLoop);
            })
        .addFunction("Halt",
            +[](int channel)
            {
                AudioSystem::Halt(channel);
            })
        .addFunction("SetVolume",
            +[](int channel, float volume)
            {
                AudioSystem::SetVolume(channel, volume);
            })
        .endNamespace()
        .beginNamespace("Image")
        .addFunction("DrawUI",
            +[](const std::string& image_name, float x, float y)
            {
                ImageSystem::DrawUI(image_name, x, y);
            })
        .addFunction("DrawUIEx",
            +[](const std::string& image_name, float x, float y,
                float r, float g, float b, float a, float sort)
            {
                ImageSystem::DrawUIEx(image_name, x, y, r, g, b, a, sort);
            })
        .addFunction("Draw",
            +[](const std::string& image_name, float x, float y)
            {
                ImageSystem::Draw(image_name, x, y);
            })
        .addFunction("DrawEx",
            +[](const std::string& image_name,
                float xx, float yy,
                float rotation_degrees, float scale_x, float scale_y,
                float pivot_x, float pivot_y,
                float rr, float gg, float bb, float aa, float sorting_order)
            {
                ImageSystem::DrawEx(image_name, xx, yy,
                    rotation_degrees, scale_x, scale_y,
                    pivot_x, pivot_y,
                    rr, gg, bb, aa, sorting_order);
            })
        .addFunction("DrawPixel",
            +[](float x, float y, float r, float g, float b, float a)
            {
                ImageSystem::DrawPixel(x, y, r, g, b, a);
            })
        .endNamespace()
        .beginNamespace("Scene")
        .addFunction("Load",
            +[](const std::string& scn) { Scene_Load(scn); })
        .addFunction("DontDestroy",
            +[](Actor* a) { Scene_DontDestroy(a); })
        .addFunction("GetCurrent",
            +[]() { return Scene_GetCurrent(); })
        .endNamespace()
        .beginNamespace("Wwise")
        .addFunction("LoadBank",
            +[](const std::string& bankName) {
                return WwiseAudioSystem::LoadBank(bankName);
            })
        .addFunction("PostEvent",
            +[](const std::string& eventName, AkGameObjectID objID) {
                WwiseAudioSystem::PostEvent(eventName, objID);
            })
        .addFunction("RegisterGameObj",
            +[](AkGameObjectID objID, const std::string& name) {
                WwiseAudioSystem::RegisterGameObj(objID, name);
            })
        .addFunction("UnregisterGameObj",
            +[](AkGameObjectID objID) {
                WwiseAudioSystem::UnregisterGameObj(objID);
            })
        .addFunction("SetRTPCValue",
            +[](const std::string& rtpcName, float value, AkGameObjectID objID) {
                WwiseAudioSystem::SetRTPCValue(rtpcName, value, objID);
            })
        .addFunction("SetSwitch",
            +[](const std::string& switchGroup, const std::string& switchState, AkGameObjectID objID) {
                WwiseAudioSystem::SetSwitch(switchGroup, switchState, objID);
            })
        .addFunction("SetState",
            +[](const std::string& group, const std::string& state) {
                WwiseAudioSystem::SetState(group, state);
            })
        .addFunction("SetGameObjectPosition",
            +[](AkGameObjectID id, float x, float y, float z) {
                WwiseAudioSystem::SetGameObjectPosition(id, x, y, z);
            })
        .addFunction("SetListener",
            +[](AkGameObjectID id) {
                WwiseAudioSystem::SetListener(id);
            })
        .endNamespace();

    Scene scene;
    std::string scenePath = "resources/scenes/basic.scene";
    g_currentSceneName = "basic";

    if (!std::filesystem::exists(scenePath))
    {
        std::cout << "error: no scene found at " << scenePath;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        WwiseAudioSystem::Shutdown();
        AudioHelper::Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 0;
    }
    scene.loadFromFile(scenePath);
    BuildSceneActorsAndComponents(scene, scenePath);

    float demoTime = 0.0f;

    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        // poll events
        while (Helper::SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = true;
            Input::ProcessEvent(event);
        }

        // onstart
        for (auto* actor : scene.actors)
        {
            if (!actor || actor->destroyed)
                continue;

            for (size_t i = 0; i < actor->pendingComponentKeysToAdd.size(); i++)
            {
                const auto& cKey = actor->pendingComponentKeysToAdd[i];
                auto cRef = actor->pendingComponentRefsToAdd[i];
                actor->components.emplace(cKey, cRef);
            }
            actor->pendingComponentKeysToAdd.clear();
            actor->pendingComponentRefsToAdd.clear();

            std::vector<std::string> keys;
            keys.reserve(actor->components.size());
            for (auto& kv : actor->components)
            {
                keys.push_back(kv.first);
            }
            std::sort(keys.begin(), keys.end());

            for (auto& k : keys)
            {
                auto refPtr = actor->components.at(k);
                if (!refPtr->isTable() && !refPtr->isUserdata())
                    continue;

                bool enabled = true;
                if ((*refPtr)["enabled"].isBool())
                {
                    enabled = (*refPtr)["enabled"].cast<bool>();
                }
                bool hasStarted = false;
                if ((*refPtr)["__has_started"].isBool())
                {
                    hasStarted = (*refPtr)["__has_started"].cast<bool>();
                }

                if (enabled && !hasStarted)
                {
                    if (refPtr->isUserdata())
                    {
                        using namespace luabridge;

                        if (refPtr->isInstance<Rigidbody>())
                        {
                            Rigidbody* rb = refPtr->cast<Rigidbody*>();
                            if (rb)
                            {
                                rb->actorOwner = actor;
                                rb->OnStart();
                            }
                        }
                        else if (refPtr->isInstance<ParticleSystem>())
                        {
                            ParticleSystem* ps = refPtr->cast<ParticleSystem*>();
                            if (ps)
                            {
                                ps->actorOwner = actor;
                                ps->OnStart();
                            }
                        }
                    }
                    if (refPtr->isTable())
                    {
                        (*refPtr)["actor"] = actor;
                    }
                    if (refPtr->isTable() && (*refPtr)["OnStart"].isFunction())
                    {
                        try
                        {
                            (*refPtr)["OnStart"](*refPtr);
                        }
                        catch (const luabridge::LuaException& e)
                        {
                            ReportError(actor->name, e);
                        }
                    }
                    else if (refPtr->isUserdata())
                    {
                        // do nothing
                    }
                    (*refPtr)["__has_started"] = true;
                }
            }
        }

        // onupdate
        for (auto* actor : scene.actors)
        {
            if (!actor || actor->destroyed)
                continue;

            std::vector<std::string> keys;
            keys.reserve(actor->components.size());
            for (auto& kv : actor->components)
            {
                keys.push_back(kv.first);
            }
            std::sort(keys.begin(), keys.end());

            for (auto& k : keys)
            {
                auto refPtr = actor->components.at(k);
                if (refPtr->isTable() && (*refPtr)["OnUpdate"].isFunction())
                {
                    bool enabled = true;
                    if ((*refPtr)["enabled"].isBool())
                    {
                        enabled = (*refPtr)["enabled"].cast<bool>();
                    }
                    if (!enabled)
                        continue;

                    try
                    {
                        (*refPtr)["OnUpdate"](*refPtr);
                    }
                    catch (const luabridge::LuaException& e)
                    {
                        ReportError(actor->name, e);
                    }
                }
                else if (refPtr->isUserdata())
                {
                    using namespace luabridge;
                    if (refPtr->isInstance<Rigidbody>())
                    {
                        // do nothing
                    }
                    else if (refPtr->isInstance<ParticleSystem>())
                    {
                        ParticleSystem* ps = refPtr->cast<ParticleSystem*>();
                        if (ps && ps->enabled)
                        {
                            ps->OnUpdate();
                        }
                    }
                }
            }
        }

        // on lateupdate
        for (auto* actor : scene.actors)
        {
            if (!actor || actor->destroyed)
                continue;

            std::vector<std::string> keys;
            keys.reserve(actor->components.size());
            for (auto& kv : actor->components)
            {
                keys.push_back(kv.first);
            }
            std::sort(keys.begin(), keys.end());

            for (auto& k : keys)
            {
                auto refPtr = actor->components.at(k);
                if (refPtr->isTable() && (*refPtr)["OnLateUpdate"].isFunction())
                {
                    bool enabled = true;
                    if ((*refPtr)["enabled"].isBool())
                    {
                        enabled = (*refPtr)["enabled"].cast<bool>();
                    }
                    if (!enabled)
                        continue;

                    try
                    {
                        (*refPtr)["OnLateUpdate"](*refPtr);
                    }
                    catch (const luabridge::LuaException& e)
                    {
                        ReportError(actor->name, e);
                    }
                }
            }
        }

        if (g_physicsWorld)
        {
            g_physicsWorld->Step(1.0f / 60.0f, 8, 3);
        }

        // remove components
        for (auto* actor : scene.actors)
        {
            if (!actor || actor->destroyed)
                continue;
            for (auto& keyToRemove : actor->pendingComponentKeysToRemove)
            {
                actor->components.erase(keyToRemove);
            }
            actor->pendingComponentKeysToRemove.clear();
        }

        demoTime += 1.0f / 60.0f;
        float t = std::fmod(demoTime, 1.0f);
        WwiseAudioSystem::SetRTPCValue("DemoTime", t, PLAYER_ID);
        WwiseAudioSystem::SetGameObjectPosition(PLAYER_ID, 0.0f, 0.0f, 0.0f);

        WwiseAudioSystem::Update();
        // render
        SDL_SetRenderDrawColor(renderer, clearColorR, clearColorG, clearColorB, 255);
        SDL_RenderClear(renderer);
        scene.render();
        ImageSystem::FlushScene(renderer);
        ImageSystem::FlushUI(renderer);
        TextSystem::Flush(renderer);
        ImageSystem::FlushPixels(renderer);
        Helper::SDL_RenderPresent(renderer);
        // check if next scene load
        Scene_LoadNextSceneIfRequested(scene);

        // postframe
        Input::LateUpdate();
        scene.RemoveDestroyedActors();
        if (!scene.pendingActorsToAdd.empty())
        {
            for (auto* newOne : scene.pendingActorsToAdd)
            {
                scene.actors.push_back(newOne);
            }
            scene.pendingActorsToAdd.clear();
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    WwiseAudioSystem::Shutdown();
    if (g_physicsWorld)
    {
        delete g_physicsWorld;
        g_physicsWorld = nullptr;
    }
    AudioHelper::Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

static void ReportError(const std::string& actor_name, const luabridge::LuaException& e)
{
    std::string error_message = e.what();
    std::replace(error_message.begin(), error_message.end(), '\\', '/');
    std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
}

static void InjectPropertiesIntoLuaRef(std::shared_ptr<luabridge::LuaRef>& instancePtr,
    const rapidjson::Value& compData)
{
    for (auto pitr = compData.MemberBegin(); pitr != compData.MemberEnd(); ++pitr)
    {
        const std::string propName = pitr->name.GetString();
        if (propName == "type")
            continue;

        if (pitr->value.IsInt())
        {
            (*instancePtr)[propName.c_str()] = pitr->value.GetInt();
        }
        else if (pitr->value.IsDouble())
        {
            (*instancePtr)[propName.c_str()] = static_cast<float>(pitr->value.GetDouble());
        }
        else if (pitr->value.IsBool())
        {
            (*instancePtr)[propName.c_str()] = pitr->value.GetBool();
        }
        else if (pitr->value.IsString())
        {
            (*instancePtr)[propName.c_str()] = pitr->value.GetString();
        }
    }
}

static bool LoadActorTemplateJson(const std::string& templateID, rapidjson::Document& outDoc)
{
    std::string templatePath = "resources/actor_templates/" + templateID + ".template";
    if (!std::filesystem::exists(templatePath))
    {
        return false;
    }
    FILE* fp = nullptr;
#ifdef _WIN32
    fopen_s(&fp, templatePath.c_str(), "rb");
#else
    fp = fopen(templatePath.c_str(), "rb");
#endif
    if (!fp)
    {
        std::cout << "Failed to open template file: " << templatePath;
        return false;
    }
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    outDoc.ParseStream(is);
    fclose(fp);

    if (outDoc.HasParseError())
    {
        std::cout << "Error parsing template file: " << templatePath;
        return false;
    }
    return true;
}

static Actor* Actor_Instantiate(const std::string& actor_template_name)
{
    if (!Scene::g_currentScene)
        return nullptr;

    Actor* newActor = new Actor();
    newActor->id = -1;
    newActor->templateID = actor_template_name;
    newActor->name = "InstantiatedActor";

    rapidjson::Document tdoc;
    if (!LoadActorTemplateJson(actor_template_name, tdoc))
    {
        Scene::g_currentScene->AddActor(newActor);
        return newActor;
    }

    if (tdoc.HasMember("actors") && tdoc["actors"].IsArray() && tdoc["actors"].Size() > 0)
    {
        const auto& templateActor = tdoc["actors"][0];
        if (templateActor.HasMember("name") && templateActor["name"].IsString())
        {
            newActor->name = templateActor["name"].GetString();
        }
        if (templateActor.HasMember("components") && templateActor["components"].IsObject())
        {
            std::vector<std::string> templateCompKeys;
            for (auto itr = templateActor["components"].MemberBegin();
                itr != templateActor["components"].MemberEnd(); ++itr)
            {
                templateCompKeys.push_back(itr->name.GetString());
            }
            std::sort(templateCompKeys.begin(), templateCompKeys.end());

            for (auto& tkey : templateCompKeys)
            {
                const auto& tcompData = templateActor["components"][tkey.c_str()];
                if (!tcompData.IsObject()) continue;
                if (!tcompData.HasMember("type") || !tcompData["type"].IsString()) continue;

                std::string tcompType = tcompData["type"].GetString();
                auto tInstancePtr = ComponentDB::CreateInstance(tcompType, tkey);
                (*tInstancePtr)["__script_type"] = tcompType;
                (*tInstancePtr)["enabled"] = true;
                (*tInstancePtr)["__has_started"] = false;
                InjectPropertiesIntoLuaRef(tInstancePtr, tcompData);
                newActor->components.emplace(tkey, tInstancePtr);
            }
        }
    }
    else if (tdoc.HasMember("components") && tdoc["components"].IsObject())
    {
        if (tdoc.HasMember("name") && tdoc["name"].IsString())
        {
            newActor->name = tdoc["name"].GetString();
        }
        std::vector<std::string> templateCompKeys;
        for (auto itr = tdoc["components"].MemberBegin();
            itr != tdoc["components"].MemberEnd(); ++itr)
        {
            templateCompKeys.push_back(itr->name.GetString());
        }
        std::sort(templateCompKeys.begin(), templateCompKeys.end());

        for (auto& tkey : templateCompKeys)
        {
            const auto& tcompData = tdoc["components"][tkey.c_str()];
            if (!tcompData.IsObject()) continue;
            if (!tcompData.HasMember("type") || !tcompData["type"].IsString()) continue;

            std::string tcompType = tcompData["type"].GetString();
            auto tInstancePtr = ComponentDB::CreateInstance(tcompType, tkey);
            (*tInstancePtr)["__script_type"] = tcompType;
            (*tInstancePtr)["enabled"] = true;
            (*tInstancePtr)["__has_started"] = false;
            InjectPropertiesIntoLuaRef(tInstancePtr, tcompData);
            newActor->components.emplace(tkey, tInstancePtr);
        }
    }

    Scene::g_currentScene->AddActor(newActor);
    return newActor;
}

static void Actor_Destroy(Actor* actor)
{
    if (!actor)
        return;
    actor->destroyed = true;
    for (auto& kv : actor->components)
    {
        if (kv.second)
        {
            (*kv.second)["enabled"] = false;
        }
    }
}

static void Scene_Load(const std::string& sceneName)
{
    s_wantsToLoadScene = true;
    s_nextSceneName = sceneName;
}

static std::string Scene_GetCurrent()
{
    return g_currentSceneName;
}

static void Scene_DontDestroy(Actor* actor)
{
    if (actor)
    {
        actor->dontDestroyOnLoad = true;
    }
}

static void Scene_LoadNextSceneIfRequested(Scene& currentScene)
{
    if (!s_wantsToLoadScene)
        return;

    std::string newScenePath = "resources/scenes/" + s_nextSceneName + ".scene";
    if (!std::filesystem::exists(newScenePath))
    {
        std::cout << "[Scene] Could not find file: " << newScenePath << std::endl;
        s_wantsToLoadScene = false;
        return;
    }

    Scene tempNew;
    if (!tempNew.loadFromFile(newScenePath))
    {
        std::cout << "[Scene] Failed to load " << newScenePath << std::endl;
        s_wantsToLoadScene = false;
        return;
    }
    BuildSceneActorsAndComponents(tempNew, newScenePath);

    std::vector<Actor*> keepers;
    keepers.reserve(currentScene.actors.size());
    for (auto* a : currentScene.actors)
    {
        if (!a)
            continue;
        if (a->dontDestroyOnLoad)
        {
            keepers.push_back(a);
        }
        else
        {
            delete a;
        }
    }
    currentScene.actors.clear();
    std::vector<Actor*> survivors;
    survivors.reserve(tempNew.actors.size());
    for (auto* newActor : tempNew.actors)
    {
        if (!newActor)
            continue;
        survivors.push_back(newActor);
    }
    tempNew.actors = std::move(survivors);

    currentScene = std::move(tempNew);
    for (auto* k : keepers)
    {
        currentScene.actors.push_back(k);
    }
    Scene::g_currentScene = &currentScene;
    g_currentSceneName = s_nextSceneName;
    s_wantsToLoadScene = false;
}

static void Application_Quit()
{
    std::cout << std::flush;
    exit(0);
}

static void Application_Sleep(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

static int Application_GetFrame()
{
    return Helper::GetFrameNumber();
}

static void Application_OpenURL(const std::string& url)
{
#ifdef _WIN32
    std::string cmd = "start " + url;
#elif __APPLE__
    std::string cmd = "open " + url;
#else
    std::string cmd = "xdg-open " + url;
#endif
    std::system(cmd.c_str());
}

static bool BuildSceneActorsAndComponents(Scene& scene, const std::string& scenePath)
{
    if (!std::filesystem::exists(scenePath))
        return false;

    FILE* fp = nullptr;
#ifdef _WIN32
    fopen_s(&fp, scenePath.c_str(), "rb");
#else
    fp = fopen(scenePath.c_str(), "rb");
#endif
    if (!fp)
        return false;

    char buffer[65536];
    rapidjson::FileReadStream fileStream(fp, buffer, sizeof(buffer));
    rapidjson::Document doc;
    doc.ParseStream(fileStream);
    fclose(fp);

    if (doc.HasParseError() || !doc.HasMember("actors") || !doc["actors"].IsArray())
    {
        std::cout << "[Scene] Invalid or missing 'actors' array in: " << scenePath << std::endl;
        return false;
    }

    const auto& actorsJson = doc["actors"];
    size_t actorIndex = 0;
    for (rapidjson::SizeType i = 0; i < actorsJson.Size(); i++)
    {
        const auto& actorObj = actorsJson[i];
        if (!actorObj.IsObject())
            continue;

        if (actorIndex >= scene.actors.size())
            break;

        Actor* actor = scene.actors[actorIndex];
        actorIndex++;

        if (!actor)
            continue;

        {
            rapidjson::Document tdoc;
            if (LoadActorTemplateJson(actor->templateID, tdoc))
            {
                if (tdoc.HasMember("actors") && tdoc["actors"].IsArray() && tdoc["actors"].Size() > 0)
                {
                    const auto& templateActor = tdoc["actors"][0];
                    if (templateActor.HasMember("components") && templateActor["components"].IsObject())
                    {
                        std::vector<std::string> templateCompKeys;
                        for (auto itr = templateActor["components"].MemberBegin();
                            itr != templateActor["components"].MemberEnd(); ++itr)
                        {
                            templateCompKeys.push_back(itr->name.GetString());
                        }
                        std::sort(templateCompKeys.begin(), templateCompKeys.end());
                        for (auto& tkey : templateCompKeys)
                        {
                            const auto& tcompData = templateActor["components"][tkey.c_str()];
                            if (!tcompData.IsObject()) continue;
                            if (!tcompData.HasMember("type") || !tcompData["type"].IsString()) continue;

                            std::string tcompType = tcompData["type"].GetString();
                            auto tInstancePtr = ComponentDB::CreateInstance(tcompType, tkey);
                            (*tInstancePtr)["__script_type"] = tcompType;
                            (*tInstancePtr)["enabled"] = true;
                            (*tInstancePtr)["__has_started"] = false;
                            InjectPropertiesIntoLuaRef(tInstancePtr, tcompData);

                            if (actor->components.find(tkey) == actor->components.end())
                            {
                                actor->components.emplace(tkey, tInstancePtr);
                            }
                            else
                            {
                                auto childPtr = actor->components.at(tkey);
                                ComponentDB::EstablishInheritance(*childPtr, *tInstancePtr);
                            }
                        }
                    }
                }
                else if (tdoc.HasMember("components") && tdoc["components"].IsObject())
                {
                    std::vector<std::string> templateCompKeys;
                    for (auto itr = tdoc["components"].MemberBegin();
                        itr != tdoc["components"].MemberEnd(); ++itr)
                    {
                        templateCompKeys.push_back(itr->name.GetString());
                    }
                    std::sort(templateCompKeys.begin(), templateCompKeys.end());
                    for (auto& tkey : templateCompKeys)
                    {
                        const auto& tcompData = tdoc["components"][tkey.c_str()];
                        if (!tcompData.IsObject()) continue;
                        if (!tcompData.HasMember("type") || !tcompData["type"].IsString()) continue;

                        std::string tcompType = tcompData["type"].GetString();
                        auto tInstancePtr = ComponentDB::CreateInstance(tcompType, tkey);
                        (*tInstancePtr)["__script_type"] = tcompType;
                        (*tInstancePtr)["enabled"] = true;
                        (*tInstancePtr)["__has_started"] = false;
                        InjectPropertiesIntoLuaRef(tInstancePtr, tcompData);

                        if (actor->components.find(tkey) == actor->components.end())
                        {
                            actor->components.emplace(tkey, tInstancePtr);
                        }
                        else
                        {
                            auto childPtr = actor->components.at(tkey);
                            ComponentDB::EstablishInheritance(*childPtr, *tInstancePtr);
                        }
                    }
                }
            }
        }

        if (actorObj.HasMember("components") && actorObj["components"].IsObject())
        {
            std::vector<std::string> compKeys;
            for (auto itr = actorObj["components"].MemberBegin();
                itr != actorObj["components"].MemberEnd(); ++itr)
            {
                compKeys.push_back(itr->name.GetString());
            }
            std::sort(compKeys.begin(), compKeys.end());

            for (auto& key : compKeys)
            {
                const auto& compData = actorObj["components"][key.c_str()];
                if (!compData.IsObject())
                    continue;

                auto found = actor->components.find(key);
                std::shared_ptr<luabridge::LuaRef> instancePtr;

                if (found == actor->components.end())
                {
                    if (!compData.HasMember("type") || !compData["type"].IsString())
                    {
                        std::cout << "error: component " << key
                            << " missing 'type' string.\n";
                        exit(0);
                    }
                    std::string compType = compData["type"].GetString();
                    auto newInst = ComponentDB::CreateInstance(compType, key);
                    (*newInst)["__script_type"] = compType;
                    (*newInst)["enabled"] = true;
                    (*newInst)["__has_started"] = false;
                    actor->components.emplace(key, newInst);

                    instancePtr = newInst;
                }
                else
                {
                    instancePtr = found->second;
                }
                InjectPropertiesIntoLuaRef(instancePtr, compData);
            }
        }
    }

    return true;
}
