#include "Rigidbody.h"
#include "box2d/box2d.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

extern b2World* g_physicsWorld;

static const float DEG2RAD = b2_pi / 180.0f;
static const float RAD2DEG = 180.0f / b2_pi;

void Rigidbody::OnStart()
{
    if (!g_physicsWorld || body != nullptr)
    {
        return;
    }

    b2BodyDef def;
    if (body_type == "dynamic")
        def.type = b2_dynamicBody;
    else if (body_type == "static")
        def.type = b2_staticBody;
    else if (body_type == "kinematic")
        def.type = b2_kinematicBody;
    else
        def.type = b2_dynamicBody;

    def.position.Set(x, y);
    def.bullet = precise;
    def.angularDamping = angular_friction;
    def.gravityScale = gravity_scale;
    def.angle = rotation * DEG2RAD;

    body = g_physicsWorld->CreateBody(&def);

    if (!has_collider && !has_trigger)
    {
        b2PolygonShape phantom_shape;
        phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);

        b2FixtureDef phantom_fixture_def;
        phantom_fixture_def.shape = &phantom_shape;
        phantom_fixture_def.density = density;
        phantom_fixture_def.isSensor = true;
        phantom_fixture_def.friction = friction;
        phantom_fixture_def.restitution = bounciness;
        phantom_fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actorOwner);
        phantom_fixture_def.filter.categoryBits = 0x0004;
        phantom_fixture_def.filter.maskBits = 0x0000;
        body->CreateFixture(&phantom_fixture_def);
        return;
    }

    if (has_collider)
    {
        b2FixtureDef fd;
        fd.density     = density;
        fd.friction    = friction;
        fd.restitution = bounciness;
        fd.isSensor    = false;
        fd.userData.pointer = reinterpret_cast<uintptr_t>(actorOwner);
        fd.filter.categoryBits = 0x0001;
        fd.filter.maskBits     = 0x0001;
        if (collider_type == "circle")
        {
            b2CircleShape circle;
            circle.m_radius = radius;
            fd.shape = &circle;
            body->CreateFixture(&fd);
        }
        else
        {
            b2PolygonShape box;
            box.SetAsBox(width * 0.5f, height * 0.5f);
            fd.shape = &box;
            body->CreateFixture(&fd);
        }
    }
    if (has_trigger)
    {
        b2FixtureDef trigger_fd;
        trigger_fd.density     = density;
        trigger_fd.friction    = friction;
        trigger_fd.restitution = bounciness;
        trigger_fd.isSensor    = true;
        trigger_fd.userData.pointer = reinterpret_cast<uintptr_t>(actorOwner);
        trigger_fd.filter.categoryBits = 0x0002;
        trigger_fd.filter.maskBits     = 0x0002;
        if (trigger_type == "circle")
        {
            b2CircleShape circle;
            circle.m_radius = trigger_radius;
            trigger_fd.shape = &circle;
            body->CreateFixture(&trigger_fd);
        }
        else
        {
            b2PolygonShape box;
            box.SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
            trigger_fd.shape = &box;
            body->CreateFixture(&trigger_fd);
        }
    }
}

b2Vec2 Rigidbody::GetPosition() const
{
    if (body)
    {
        return body->GetPosition();
    }
    return b2Vec2(x, y);
}

float Rigidbody::GetRotation() const
{
    if (body)
    {
        float angleRadians = body->GetAngle();
        return angleRadians * RAD2DEG;
    }
    return rotation;
}

void Rigidbody::AddForce(const b2Vec2& force)
{
    if (body)
    {
        body->ApplyForceToCenter(force, true);
    }
}

void Rigidbody::SetVelocity(const b2Vec2& vel)
{
    if (body)
    {
        body->SetLinearVelocity(vel);
    }
}

void Rigidbody::SetPosition(const b2Vec2& pos)
{
    if (body)
    {
        float oldAngle = body->GetAngle();
        body->SetTransform(pos, oldAngle);
    }
}

void Rigidbody::SetRotation(float degrees_clockwise)
{
    if (body)
    {
        b2Vec2 oldPos = body->GetPosition();
        float newAngleRadians = degrees_clockwise * DEG2RAD;
        body->SetTransform(oldPos, newAngleRadians);
    }
}

void Rigidbody::SetAngularVelocity(float deg_per_sec_clockwise)
{
    if (body)
    {
        float radsec = deg_per_sec_clockwise * DEG2RAD;
        body->SetAngularVelocity(radsec);
    }
}

void Rigidbody::SetGravityScale(float scale)
{
    if (body)
    {
        body->SetGravityScale(scale);
    }
}

void Rigidbody::SetUpDirection(const b2Vec2& dir)
{
    if (!body) return;
    b2Vec2 ndir = dir;
    if (ndir.LengthSquared() > 0.0001f)
        ndir.Normalize();
    float newAngle = glm::atan(ndir.x, -ndir.y);
    body->SetTransform(body->GetPosition(), newAngle);
}

void Rigidbody::SetRightDirection(const b2Vec2& dir)
{
    if (!body) return;
    b2Vec2 ndir = dir;
    if (ndir.LengthSquared() > 0.0001f)
        ndir.Normalize();
    float angleAsUp = glm::atan(ndir.x, -ndir.y);
    float newAngle = angleAsUp - b2_pi * 0.5f;
    body->SetTransform(body->GetPosition(), newAngle);
}

b2Vec2 Rigidbody::GetVelocity() const
{
    if (body)
        return body->GetLinearVelocity();
    return b2Vec2(0, 0);
}

float Rigidbody::GetAngularVelocity() const
{
    if (body)
    {
        float radsec = body->GetAngularVelocity();
        return radsec * RAD2DEG;
    }
    return 0.0f;
}

float Rigidbody::GetGravityScale() const
{
    if (body)
        return body->GetGravityScale();
    return gravity_scale;
}

b2Vec2 Rigidbody::GetUpDirection() const
{
    if (body)
    {
        float angle = body->GetAngle();
        b2Vec2 up(glm::sin(angle), -glm::cos(angle));
        up.Normalize();
        return up;
    }
    return b2Vec2(0.0f, -1.0f);
}

b2Vec2 Rigidbody::GetRightDirection() const
{
    if (body)
    {
        float angle = body->GetAngle();
        b2Vec2 right(glm::cos(angle), glm::sin(angle));
        right.Normalize();
        return right;
    }
    return b2Vec2(1.0f, 0.0f);
}
