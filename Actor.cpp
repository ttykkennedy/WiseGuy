#include "Actor.h"
#include "ComponentDB.h"
#include <vector>
#include <algorithm>
#include <iostream>

static int g_runtimeComponentAddCounter = 0;

luabridge::LuaRef Actor::GetComponentByKey(const std::string& key)
{
    auto it = components.find(key);
    if (it == components.end())
    {
        return luabridge::LuaRef(ComponentDB::lua_state);
    }
    luabridge::LuaRef compTable = *(it->second);
    if (compTable["enabled"].isBool() && !compTable["enabled"].cast<bool>())
    {
        return luabridge::LuaRef(ComponentDB::lua_state);
    }
    return compTable;
}

luabridge::LuaRef Actor::GetComponent(const std::string& type_name)
{
    std::vector<std::string> sortedKeys;
    sortedKeys.reserve(components.size());
    for (auto& kv : components)
    {
        sortedKeys.push_back(kv.first);
    }
    std::sort(sortedKeys.begin(), sortedKeys.end());

    for (auto& k : sortedKeys)
    {
        auto refPtr = components.at(k);
        luabridge::LuaRef compTable = *refPtr;
        if (compTable["enabled"].isBool() && !compTable["enabled"].cast<bool>())
        {
            continue;
        }
        if (compTable["__script_type"].isString())
        {
            std::string st = compTable["__script_type"].cast<std::string>();
            if (st == type_name)
            {
                return compTable;
            }
        }
    }
    return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef Actor::GetComponents(const std::string& type_name)
{
    luabridge::LuaRef arr = luabridge::newTable(ComponentDB::lua_state);

    std::vector<std::string> sortedKeys;
    sortedKeys.reserve(components.size());
    for (auto& kv : components)
    {
        sortedKeys.push_back(kv.first);
    }
    std::sort(sortedKeys.begin(), sortedKeys.end());

    int insertIndex = 1;
    for (auto& k : sortedKeys)
    {
        auto refPtr = components.at(k);
        luabridge::LuaRef compTable = *refPtr;
        if (compTable["enabled"].isBool() && !compTable["enabled"].cast<bool>())
        {
            continue;
        }
        if (compTable["__script_type"].isString())
        {
            std::string st = compTable["__script_type"].cast<std::string>();
            if (st == type_name)
            {
                arr[insertIndex++] = compTable;
            }
        }
    }
    return arr;
}

luabridge::LuaRef Actor::AddComponent(const std::string& type_name)
{
    int index = g_runtimeComponentAddCounter++;
    std::string runtimeKey = "r" + std::to_string(index);

    auto compPtr = ComponentDB::CreateInstance(type_name, runtimeKey);
    (*compPtr)["__script_type"] = type_name;
    (*compPtr)["enabled"] = true;
    (*compPtr)["__has_started"] = false;

    pendingComponentKeysToAdd.push_back(runtimeKey);
    pendingComponentRefsToAdd.push_back(compPtr);

    return *compPtr;
}

void Actor::RemoveComponent(luabridge::LuaRef componentRef)
{
    if (!componentRef.isTable() && !componentRef.isUserdata())
        return;

    componentRef["enabled"] = false;
    if (componentRef["key"].isString())
    {
        std::string compKey = componentRef["key"].cast<std::string>();
        pendingComponentKeysToRemove.push_back(compKey);
    }
}
