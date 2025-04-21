#ifndef ACTOR_H
#define ACTOR_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

struct Actor
{
    int id = 0;
    std::string name;
    std::string templateID;
    bool dontDestroyOnLoad = false;
    std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> components;
    bool destroyed = false;
    std::vector<std::string> pendingComponentKeysToAdd;
    std::vector<std::string> pendingComponentKeysToRemove;
    std::vector<std::shared_ptr<luabridge::LuaRef>> pendingComponentRefsToAdd;
    std::string GetName() const { return name; }
    int GetID() const { return id; }

    luabridge::LuaRef GetComponentByKey(const std::string& key);
    luabridge::LuaRef GetComponent(const std::string& type_name);
    luabridge::LuaRef GetComponents(const std::string& type_name);
    luabridge::LuaRef AddComponent(const std::string& type_name);
    void RemoveComponent(luabridge::LuaRef componentRef);
};

#endif
