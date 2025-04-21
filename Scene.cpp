#include "Scene.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <filesystem>
#include <iostream>

Scene* Scene::g_currentScene = nullptr;
bool Scene::loadFromFile(const std::string& filePath)
{
    if (!std::filesystem::exists(filePath))
    {
        std::cout << "Scene file " << filePath << " does not exist.\n";
        return false;
    }
    FILE* fp = nullptr;
#ifdef _WIN32
    fopen_s(&fp, filePath.c_str(), "rb");
#else
    fp = fopen(filePath.c_str(), "rb");
#endif
    if (!fp)
    {
        std::cout << "Failed to open scene file: " << filePath << std::endl;
        return false;
    }
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    if (doc.HasParseError())
    {
        std::cout << "Error parsing scene file: " << filePath << std::endl;
        return false;
    }

    if (!doc.HasMember("actors") || !doc["actors"].IsArray())
    {
        std::cout << "Scene file does not contain an actors array.\n";
        return false;
    }

    const rapidjson::Value& actorsArray = doc["actors"];
    actors.reserve(actorsArray.Size());
    int actor_id_counter = 0;

    for (rapidjson::SizeType i = 0; i < actorsArray.Size(); i++)
    {
        const rapidjson::Value& actorObj = actorsArray[i];
        if (!actorObj.IsObject()) continue;

        Actor* actor = new Actor();
        actor->id = actor_id_counter++;

        if (actorObj.HasMember("name") && actorObj["name"].IsString())
            actor->name = actorObj["name"].GetString();

        if (actorObj.HasMember("template") && actorObj["template"].IsString())
            actor->templateID = actorObj["template"].GetString();

        actors.push_back(actor);
    }
    {
        std::filesystem::path p(filePath);
        m_sceneName = p.stem().string();
    }
    Scene::g_currentScene = this;
    return true;
}

void Scene::render()
{
    // do nothing
}

void Scene::AddActor(Actor* actor)
{
    pendingActorsToAdd.push_back(actor);
}

void Scene::RemoveDestroyedActors()
{
    std::vector<Actor*> survivors;
    survivors.reserve(actors.size());
    for (auto* actor : actors)
    {
        if (actor->destroyed)
        {
            delete actor;
        }
        else
        {
            survivors.push_back(actor);
        }
    }
    actors = std::move(survivors);
}

Scene::Scene(Scene&& other) noexcept
{
    actors             = std::move(other.actors);
    pendingActorsToAdd = std::move(other.pendingActorsToAdd);
    m_sceneName        = std::move(other.m_sceneName);

    other.actors.clear();
    other.pendingActorsToAdd.clear();
    other.m_sceneName.clear();
}

Scene& Scene::operator=(Scene&& other) noexcept
{
    if (this != &other)
    {
        for (auto* actor : actors)
            delete actor;
        actors.clear();

        for (auto* actor : pendingActorsToAdd)
            delete actor;
        pendingActorsToAdd.clear();

        actors             = std::move(other.actors);
        pendingActorsToAdd = std::move(other.pendingActorsToAdd);
        m_sceneName        = std::move(other.m_sceneName);

        other.actors.clear();
        other.pendingActorsToAdd.clear();
        other.m_sceneName.clear();
    }
    return *this;
}
Scene::~Scene()
{
    for (auto* actor : actors)
    {
        delete actor;
    }
    actors.clear();

    for (auto* actor : pendingActorsToAdd)
    {
        delete actor;
    }
    pendingActorsToAdd.clear();
}
