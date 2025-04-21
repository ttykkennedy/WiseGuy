#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "Actor.h"

class Scene
{
public:
    std::vector<Actor*> actors;
    static Scene* g_currentScene;

    std::string m_sceneName;
    bool loadFromFile(const std::string& filePath);
    void render();

    std::vector<Actor*> pendingActorsToAdd;
    void AddActor(Actor* actor);
    void RemoveDestroyedActors();

    Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Scene(Scene&& other) noexcept;
    Scene& operator=(Scene&& other) noexcept;
    ~Scene();
};

#endif
