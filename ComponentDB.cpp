#include "ComponentDB.h"
#include <filesystem>
#include "Camera.h"
#include "Scene.h"
#include "box2d/box2d.h"
#include "Rigidbody.h"
#include <algorithm>
#include "Actor.h"
#include "ParticleSystem.h"

b2World* g_physicsWorld = nullptr;
static bool s_hasEverCreatedRigidbody = false;

class MyContactListener : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact) override
    {
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();
        if (!fixtureA || !fixtureB) return;

        Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
        Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
        if (!actorA || !actorB) return;

        bool sensorA = fixtureA->IsSensor();
        bool sensorB = fixtureB->IsSensor();

        if (!sensorA && !sensorB)
        {
            b2WorldManifold worldManifold;
            contact->GetWorldManifold(&worldManifold);

            b2Vec2 normal = worldManifold.normal;
            b2Vec2 pt(0, 0);
            if (contact->GetManifold()->pointCount > 0)
            {
                pt = worldManifold.points[0];
            }

            b2Vec2 relativeVel = fixtureA->GetBody()->GetLinearVelocity()
                                 - fixtureB->GetBody()->GetLinearVelocity();

            DispatchCollisionEvent(actorA, actorB, pt, normal, relativeVel, true);
            DispatchCollisionEvent(actorB, actorA, pt, normal, relativeVel, true);
        }
        else if (sensorA && sensorB)
        {
            b2Vec2 sentinel(-999.0f, -999.0f);
            b2Vec2 relativeVel = fixtureA->GetBody()->GetLinearVelocity()
                                 - fixtureB->GetBody()->GetLinearVelocity();

            DispatchTriggerEvent(actorA, actorB, sentinel, sentinel, relativeVel, true);
            DispatchTriggerEvent(actorB, actorA, sentinel, sentinel, relativeVel, true);
        }
        else
        {
            // do nothing 4 now
        }
    }

    void EndContact(b2Contact* contact) override
    {
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();
        if (!fixtureA || !fixtureB) return;

        Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
        Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
        if (!actorA || !actorB) return;

        bool sensorA = fixtureA->IsSensor();
        bool sensorB = fixtureB->IsSensor();

        if (!sensorA && !sensorB)
        {
            b2Vec2 sentinel(-999.0f, -999.0f);
            b2Vec2 relativeVel = fixtureA->GetBody()->GetLinearVelocity()
                                 - fixtureB->GetBody()->GetLinearVelocity();

            DispatchCollisionEvent(actorA, actorB, sentinel, sentinel, relativeVel, false);
            DispatchCollisionEvent(actorB, actorA, sentinel, sentinel, relativeVel, false);
        }
        else if (sensorA && sensorB)
        {
            b2Vec2 sentinel(-999.0f, -999.0f);
            b2Vec2 relativeVel = fixtureA->GetBody()->GetLinearVelocity()
                                 - fixtureB->GetBody()->GetLinearVelocity();

            DispatchTriggerEvent(actorA, actorB, sentinel, sentinel, relativeVel, false);
            DispatchTriggerEvent(actorB, actorA, sentinel, sentinel, relativeVel, false);
        }
        else
        {
            // do nothing 4 now
        }
    }

private:
    void DispatchCollisionEvent(Actor* mainActor,
                                Actor* otherActor,
                                const b2Vec2& point,
                                const b2Vec2& normal,
                                const b2Vec2& relativeVel,
                                bool isBeginContact)
    {
        auto L = ComponentDB::lua_state;
        using namespace luabridge;

        std::vector<std::string> sortedKeys;
        sortedKeys.reserve(mainActor->components.size());
        for (auto& kv : mainActor->components)
        {
            sortedKeys.push_back(kv.first);
        }
        std::sort(sortedKeys.begin(), sortedKeys.end());

        for (auto& k : sortedKeys)
        {
            auto refPtr = mainActor->components.at(k);
            if (!refPtr->isTable())
                continue;

            LuaRef compTable = *refPtr;
            if (!compTable["enabled"].isBool() || !compTable["enabled"].cast<bool>())
                continue;

            const char* funcName = isBeginContact ? "OnCollisionEnter" : "OnCollisionExit";
            if (!compTable[funcName].isFunction())
                continue;

            LuaRef collision = newTable(L);
            collision["other"] = otherActor;

            {
                LuaRef ptTable = newTable(L);
                ptTable["x"] = point.x;
                ptTable["y"] = point.y;
                collision["point"] = ptTable;
            }
            {
                LuaRef normTable = newTable(L);
                normTable["x"] = normal.x;
                normTable["y"] = normal.y;
                collision["normal"] = normTable;
            }
            {
                LuaRef rvTable = newTable(L);
                rvTable["x"] = relativeVel.x;
                rvTable["y"] = relativeVel.y;
                collision["relative_velocity"] = rvTable;
            }

            try
            {
                compTable[funcName](compTable, collision);
            }
            catch (const luabridge::LuaException& e)
            {
                std::string msg = e.what();
                std::replace(msg.begin(), msg.end(), '\\', '/');
                std::cout << "\033[31m"
                          << mainActor->name << " : " << msg
                          << "\033[0m" << std::endl;
            }
        }
    }

    void DispatchTriggerEvent(Actor* mainActor,
                              Actor* otherActor,
                              const b2Vec2& point,
                              const b2Vec2& normal,
                              const b2Vec2& relativeVel,
                              bool isBeginContact)
    {
        auto L = ComponentDB::lua_state;
        using namespace luabridge;

        std::vector<std::string> sortedKeys;
        sortedKeys.reserve(mainActor->components.size());
        for (auto& kv : mainActor->components)
        {
            sortedKeys.push_back(kv.first);
        }
        std::sort(sortedKeys.begin(), sortedKeys.end());

        for (auto& k : sortedKeys)
        {
            auto refPtr = mainActor->components.at(k);
            if (!refPtr->isTable())
                continue;

            LuaRef compTable = *refPtr;
            if (!compTable["enabled"].isBool() || !compTable["enabled"].cast<bool>())
                continue;

            const char* funcName = isBeginContact ? "OnTriggerEnter" : "OnTriggerExit";
            if (!compTable[funcName].isFunction())
                continue;

            LuaRef collision = newTable(L);
            collision["other"] = otherActor;

            {
                LuaRef ptTable = newTable(L);
                ptTable["x"] = point.x;
                ptTable["y"] = point.y;
                collision["point"] = ptTable;
            }
            {
                LuaRef normTable = newTable(L);
                normTable["x"] = normal.x;
                normTable["y"] = normal.y;
                collision["normal"] = normTable;
            }
            {
                LuaRef rvTable = newTable(L);
                rvTable["x"] = relativeVel.x;
                rvTable["y"] = relativeVel.y;
                collision["relative_velocity"] = rvTable;
            }

            try
            {
                compTable[funcName](compTable, collision);
            }
            catch (const luabridge::LuaException& e)
            {
                std::string msg = e.what();
                std::replace(msg.begin(), msg.end(), '\\', '/');
                std::cout << "\033[31m"
                          << mainActor->name << " : " << msg
                          << "\033[0m" << std::endl;
            }
        }
    }
};

static MyContactListener s_contactListener;

static b2Vec2 AddWrapper(const b2Vec2* self, const b2Vec2& other)
{
    return b2Vec2(self->x + other.x, self->y + other.y);
}
static b2Vec2 SubWrapper(const b2Vec2* self, const b2Vec2& other)
{
    return b2Vec2(self->x - other.x, self->y - other.y);
}
static b2Vec2 MulWrapper(const b2Vec2* self, float scalar)
{
    return b2Vec2(self->x * scalar, self->y * scalar);
}

void ComponentDB::InitLua()
{
    lua_state = luaL_newstate();
    if (!lua_state)
    {
        std::cout << "error: failed to create lua_State\n";
        exit(0);
    }
    luaL_openlibs(lua_state);

    using namespace luabridge;
    getGlobalNamespace(lua_state)
        .beginNamespace("Debug")
        .addFunction("Log", &ComponentDB::CppLog)
        .endNamespace()
    
        .beginNamespace("Camera")
                    .addFunction("SetPosition", &Camera::SetPosition)
                    .addFunction("GetPositionX", &Camera::GetPositionX)
                    .addFunction("GetPositionY", &Camera::GetPositionY)
                    .addFunction("SetZoom", &Camera::SetZoom)
                    .addFunction("GetZoom", &Camera::GetZoom)
                    .endNamespace()

        .beginClass<b2Vec2>("Vector2")
            .addConstructor<void(*)(float, float)>()
            .addProperty("x", &b2Vec2::x)
            .addProperty("y", &b2Vec2::y)
            .addFunction("Normalize", &b2Vec2::Normalize)
            .addFunction("Length", &b2Vec2::Length)
            .addFunction("__add", &AddWrapper)
            .addFunction("__sub", &SubWrapper)
            .addFunction("__mul", &MulWrapper)
            .addStaticFunction("Dot", static_cast<float(*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
            .addStaticFunction("Distance", static_cast<float(*)(const b2Vec2&, const b2Vec2&)>(&b2Distance))
        .endClass()
    
        .beginClass<Rigidbody>("Rigidbody")
            .addConstructor<void(*)()>()
            .addProperty("enabled", &Rigidbody::enabled)
            .addProperty("key", &Rigidbody::key)
            .addProperty("__has_started", &Rigidbody::__has_started)
            .addProperty("__script_type", &Rigidbody::__script_type)
            .addProperty("x", &Rigidbody::x)
            .addProperty("y", &Rigidbody::y)
            .addProperty("body_type", &Rigidbody::body_type)
            .addProperty("precise", &Rigidbody::precise)
            .addProperty("gravity_scale", &Rigidbody::gravity_scale)
            .addProperty("density", &Rigidbody::density)
            .addProperty("angular_friction", &Rigidbody::angular_friction)
            .addProperty("rotation", &Rigidbody::rotation)
            .addProperty("has_collider", &Rigidbody::has_collider)
            .addProperty("has_trigger", &Rigidbody::has_trigger)
            .addProperty("trigger_type", &Rigidbody::trigger_type)
            .addProperty("trigger_width", &Rigidbody::trigger_width)
            .addProperty("trigger_height", &Rigidbody::trigger_height)
            .addProperty("trigger_radius", &Rigidbody::trigger_radius)
            .addProperty("mass", &Rigidbody::mass)
            .addProperty("velocity", &Rigidbody::velocity)
            .addProperty("collider_type", &Rigidbody::collider_type)
            .addProperty("width",        &Rigidbody::width)
            .addProperty("height",       &Rigidbody::height)
            .addProperty("radius",       &Rigidbody::radius)
            .addProperty("friction",     &Rigidbody::friction)
            .addProperty("bounciness",   &Rigidbody::bounciness)
            .addFunction("GetPosition", &Rigidbody::GetPosition)
            .addFunction("GetRotation", &Rigidbody::GetRotation)
            .addFunction("AddForce", &Rigidbody::AddForce)
            .addFunction("SetVelocity", &Rigidbody::SetVelocity)
            .addFunction("SetPosition", &Rigidbody::SetPosition)
            .addFunction("SetRotation", &Rigidbody::SetRotation)
            .addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
            .addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
            .addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
            .addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
            .addFunction("GetVelocity", &Rigidbody::GetVelocity)
            .addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
            .addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
            .addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
            .addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
        .endClass()
        .beginClass<ParticleSystem>("ParticleSystem")
            .addConstructor<void(*)()>()
            .addProperty("enabled", &ParticleSystem::enabled)
            .addProperty("key", &ParticleSystem::key)
            .addProperty("__has_started", &ParticleSystem::__has_started)
            .addProperty("__script_type", &ParticleSystem::__script_type)
            .addProperty("x", &ParticleSystem::x)
            .addProperty("y", &ParticleSystem::y)
            .addProperty("emit_angle_min", &ParticleSystem::emit_angle_min)
            .addProperty("emit_angle_max", &ParticleSystem::emit_angle_max)
            .addProperty("emit_radius_min", &ParticleSystem::emit_radius_min)
            .addProperty("emit_radius_max", &ParticleSystem::emit_radius_max)
            .addProperty("frames_between_bursts", &ParticleSystem::frames_between_bursts)
            .addProperty("burst_quantity", &ParticleSystem::burst_quantity)
            .addProperty("start_scale_min", &ParticleSystem::start_scale_min)
            .addProperty("start_scale_max", &ParticleSystem::start_scale_max)
            .addProperty("rotation_min", &ParticleSystem::rotation_min)
            .addProperty("rotation_max", &ParticleSystem::rotation_max)
            .addProperty("start_color_r", &ParticleSystem::start_color_r)
            .addProperty("start_color_g", &ParticleSystem::start_color_g)
            .addProperty("start_color_b", &ParticleSystem::start_color_b)
            .addProperty("start_color_a", &ParticleSystem::start_color_a)
            .addProperty("image", &ParticleSystem::image)
            .addProperty("sorting_order", &ParticleSystem::sorting_order)
            .addProperty("duration_frames", &ParticleSystem::duration_frames)
            .addProperty("start_speed_min", &ParticleSystem::start_speed_min)
            .addProperty("start_speed_max", &ParticleSystem::start_speed_max)
            .addProperty("rotation_speed_min", &ParticleSystem::rotation_speed_min)
            .addProperty("rotation_speed_max", &ParticleSystem::rotation_speed_max)
            .addProperty("gravity_scale_x", &ParticleSystem::gravity_scale_x)
            .addProperty("gravity_scale_y", &ParticleSystem::gravity_scale_y)
            .addProperty("drag_factor", &ParticleSystem::drag_factor)
            .addProperty("angular_drag_factor", &ParticleSystem::angular_drag_factor)
            .addProperty("end_scale", &ParticleSystem::end_scale)
            .addProperty("end_color_r", &ParticleSystem::end_color_r)
            .addProperty("end_color_g", &ParticleSystem::end_color_g)
            .addProperty("end_color_b", &ParticleSystem::end_color_b)
            .addProperty("end_color_a", &ParticleSystem::end_color_a)
            .addFunction("Stop", &ParticleSystem::Stop)
            .addFunction("Play", &ParticleSystem::Play)
            .addFunction("Burst", &ParticleSystem::Burst)
        .endClass();
}

int ComponentDB::CppLog(const std::string& message)
{
    std::cout << message << std::endl;
    return 0;
}

std::shared_ptr<luabridge::LuaRef> ComponentDB::GetComponentBase(const std::string& componentName)
{
    if (componentName == "Rigidbody")
    {
        return nullptr;
    }
    auto it = cachedBases.find(componentName);
    if (it != cachedBases.end())
    {
        return it->second;
    }

    std::string luaFilename = "resources/component_types/" + componentName + ".lua";
    if (!std::filesystem::exists(luaFilename))
    {
        std::cout << "error: failed to locate component " << componentName;
        exit(0);
    }
    if (luaL_dofile(lua_state, luaFilename.c_str()) != LUA_OK)
    {
        std::filesystem::path p(luaFilename);
        std::string stem = p.stem().string();
        std::cout << "problem with lua file " << stem;
        exit(0);
    }
    luabridge::LuaRef baseTable = luabridge::getGlobal(lua_state, componentName.c_str());
    if (!baseTable.isTable())
    {
        std::filesystem::path p(luaFilename);
        std::string stem = p.stem().string();
        std::cout << "problem with lua file " << stem << " -- not a valid table";
        exit(0);
    }
    auto baseRefPtr = std::make_shared<luabridge::LuaRef>(baseTable);
    cachedBases.emplace(componentName, baseRefPtr);
    return baseRefPtr;
}

std::shared_ptr<luabridge::LuaRef> ComponentDB::CreateInstance(const std::string& componentName,
    const std::string& componentKey)
{
    using namespace luabridge;
    if (componentName == "Rigidbody")
    {
        if (!s_hasEverCreatedRigidbody)
        {
            s_hasEverCreatedRigidbody = true;
            if (!g_physicsWorld)
            {
                b2Vec2 gravity(0.0f, 9.8f);
                g_physicsWorld = new b2World(gravity);
                g_physicsWorld->SetContactListener(&s_contactListener);
            }
        }
        Rigidbody* new_rb = new Rigidbody();
        new_rb->key           = componentKey;
        new_rb->enabled       = true;
        new_rb->__has_started = false;
        new_rb->__script_type = "Rigidbody";

        LuaRef ref(lua_state, new_rb);
        return std::make_shared<LuaRef>(ref);
    }
    
    if (componentName == "ParticleSystem")
    {
        ParticleSystem* ps = new ParticleSystem();
        ps->key           = componentKey;
        ps->enabled       = true;
        ps->__has_started = false;
        ps->__script_type = "ParticleSystem";
        LuaRef ref(lua_state, ps);
        return std::make_shared<LuaRef>(ref);
    }

    auto base = GetComponentBase(componentName);
    LuaRef instance = newTable(lua_state);
    instance["key"] = componentKey;
    instance["enabled"] = true;
    instance["__has_started"] = false;

    EstablishInheritance(instance, *base);
    return std::make_shared<LuaRef>(instance);
}

void ComponentDB::EstablishInheritance(luabridge::LuaRef& instance_table,
    luabridge::LuaRef& parent_table)
{
    luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
    new_metatable["__index"] = parent_table;

    instance_table.push(lua_state);
    new_metatable.push(lua_state);
    lua_setmetatable(lua_state, -2);
    lua_pop(lua_state, 1);
}
