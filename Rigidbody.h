#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <string>
#include "box2d/box2d.h"
#include "glm/glm.hpp"

struct Actor;
class Rigidbody
{
public:
    bool enabled = true;
    std::string key;
    bool __has_started = false;
    std::string __script_type = "Rigidbody";

    float x = 0.0f;
    float y = 0.0f;
    std::string body_type = "dynamic";
    bool precise = true;
    float gravity_scale = 1.0f;
    float density = 1.0f;
    float angular_friction = 0.3f;
    float rotation = 0.0f;
    bool has_collider = true;
    bool has_trigger = true;
    std::string trigger_type = "box";
    float trigger_width = 1.0f;
    float trigger_height = 1.0f;
    float trigger_radius = 0.5f;
    float mass = 1.0f;
    b2Vec2 velocity{0.0f, 0.0f};
    std::string collider_type = "box";
    float width = 1.0f;
    float height = 1.0f;
    float radius = 0.5f;
    float friction = 0.3f;
    float bounciness = 0.3f;
    b2Body* body = nullptr;
    Actor* actorOwner = nullptr;
    Rigidbody() = default;
    Rigidbody(const Rigidbody& other)
    {
        enabled          = other.enabled;
        key              = other.key;
        __has_started    = other.__has_started;
        __script_type    = other.__script_type;
        x                = other.x;
        y                = other.y;
        body_type        = other.body_type;
        precise          = other.precise;
        gravity_scale    = other.gravity_scale;
        density          = other.density;
        angular_friction = other.angular_friction;
        rotation         = other.rotation;
        has_collider     = other.has_collider;
        has_trigger      = other.has_trigger;
        trigger_type     = other.trigger_type;
        trigger_width    = other.trigger_width;
        trigger_height   = other.trigger_height;
        trigger_radius   = other.trigger_radius;
        mass             = other.mass;
        velocity         = other.velocity;
        collider_type    = other.collider_type;
        width            = other.width;
        height           = other.height;
        radius           = other.radius;
        friction         = other.friction;
        bounciness       = other.bounciness;
        body             = nullptr;
        actorOwner       = other.actorOwner;
    }

    void OnStart();

    b2Vec2 GetPosition() const;
    float  GetRotation() const;
    void AddForce(const b2Vec2& force);
    void SetVelocity(const b2Vec2& vel);
    void SetPosition(const b2Vec2& pos);
    void SetRotation(float degrees_clockwise);
    void SetAngularVelocity(float degrees_per_sec_clockwise);
    void SetGravityScale(float scale);
    void SetUpDirection(const b2Vec2& direction);
    void SetRightDirection(const b2Vec2& direction);
    b2Vec2 GetVelocity() const;
    float  GetAngularVelocity() const;
    float  GetGravityScale() const;
    b2Vec2 GetUpDirection() const;
    b2Vec2 GetRightDirection() const;
};

#endif
