#ifndef COMPONENTDB_H
#define COMPONENTDB_H

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>

class ComponentDB
{
public:
    static void InitLua();

    static int CppLog(const std::string& message);

    static std::shared_ptr<luabridge::LuaRef> GetComponentBase(const std::string& componentName);

    static std::shared_ptr<luabridge::LuaRef> CreateInstance(const std::string& componentName,
        const std::string& componentKey);

    static void EstablishInheritance(luabridge::LuaRef& instance_table,
        luabridge::LuaRef& parent_table);

    static inline lua_State* lua_state = nullptr;

private:
    static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> cachedBases;
};

#endif
