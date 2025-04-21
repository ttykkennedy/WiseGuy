#include "ParticleSystem.h"
#include "ImageSystem.h"
#include "Rigidbody.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/random.hpp"

ParticleSystem::ParticleSystem(const ParticleSystem& other)
{
    enabled                 = other.enabled;
    key                     = other.key;
    __has_started           = other.__has_started;
    __script_type           = other.__script_type;
    actorOwner              = other.actorOwner;
    emit_angle_distribution = other.emit_angle_distribution;
    emit_radius_distribution= other.emit_radius_distribution;
    m_particles             = other.m_particles;
    std::queue<size_t> temp = other.m_freeList;
    while(!m_freeList.empty()) m_freeList.pop();
    while(!temp.empty()){
        m_freeList.push(temp.front());
        temp.pop();
    }
    x                       = other.x;
    y                       = other.y;
    emit_angle_min          = other.emit_angle_min;
    emit_angle_max          = other.emit_angle_max;
    emit_radius_min         = other.emit_radius_min;
    emit_radius_max         = other.emit_radius_max;
    frames_between_bursts   = other.frames_between_bursts;
    burst_quantity          = other.burst_quantity;
    start_scale_min         = other.start_scale_min;
    start_scale_max         = other.start_scale_max;
    rotation_min            = other.rotation_min;
    rotation_max            = other.rotation_max;
    start_color_r           = other.start_color_r;
    start_color_g           = other.start_color_g;
    start_color_b           = other.start_color_b;
    start_color_a           = other.start_color_a;
    image                   = other.image;
    sorting_order           = other.sorting_order;
    local_frame_number      = other.local_frame_number;
    rotation_distribution   = other.rotation_distribution;
    scale_distribution      = other.scale_distribution;
    duration_frames         = other.duration_frames;
    start_speed_min         = other.start_speed_min;
    start_speed_max         = other.start_speed_max;
    rotation_speed_min      = other.rotation_speed_min;
    rotation_speed_max      = other.rotation_speed_max;
    gravity_scale_x         = other.gravity_scale_x;
    gravity_scale_y         = other.gravity_scale_y;
    drag_factor             = other.drag_factor;
    angular_drag_factor     = other.angular_drag_factor;
    end_scale               = other.end_scale;
    end_color_r             = other.end_color_r;
    end_color_g             = other.end_color_g;
    end_color_b             = other.end_color_b;
    end_color_a             = other.end_color_a;
    speed_distribution      = other.speed_distribution;
    rotation_speed_distribution = other.rotation_speed_distribution;
    emission_allowed        = other.emission_allowed;
}

void ParticleSystem::OnStart()
{
    if (frames_between_bursts < 1) {
        frames_between_bursts = 1;
    }
    if (burst_quantity < 1) {
        burst_quantity = 1;
    }
    if (duration_frames < 1) {
        duration_frames = 1;
    }

    emit_angle_distribution.Configure(emit_angle_min, emit_angle_max, 298);
    emit_radius_distribution.Configure(emit_radius_min, emit_radius_max, 404);

    rotation_distribution.Configure(rotation_min, rotation_max, 440);
    scale_distribution.Configure(start_scale_min, start_scale_max, 494);

    speed_distribution.Configure(start_speed_min, start_speed_max, 498);
    rotation_speed_distribution.Configure(rotation_speed_min, rotation_speed_max, 305);

    ImageSystem::CreateDefaultParticleTexture("default_particle");
}

void ParticleSystem::OnUpdate()
{
    if (!enabled)
        return;

    float actorX = 0.0f;
    float actorY = 0.0f;
    if (actorOwner)
    {
        auto rbRef = actorOwner->GetComponent("Rigidbody");
        if (!rbRef.isNil() && rbRef.isUserdata())
        {
            auto* rb = rbRef.cast<Rigidbody*>();
            if (rb) {
                b2Vec2 pos = rb->GetPosition();
                actorX = pos.x;
                actorY = pos.y;
            }
        }
    }
    if (emission_allowed && (local_frame_number % frames_between_bursts == 0))
    {
        GenerateNewParticles(burst_quantity);
    }

    for (size_t i = 0; i < m_particles.size(); i++)
    {
        Particle& p = m_particles[i];
        if (!p.active)
            continue;

        int frames_alive = local_frame_number - p.birth_frame;
        if (frames_alive >= duration_frames)
        {
            p.active = false;
            m_freeList.push(i);
            continue;
        }

        p.vx += gravity_scale_x;
        p.vy += gravity_scale_y;

        p.vx *= drag_factor;
        p.vy *= drag_factor;
        p.angular_velocity *= angular_drag_factor;

        p.x += p.vx;
        p.y += p.vy;
        p.rotation += p.angular_velocity;

        float lifetime_progress = (float)frames_alive / (float)duration_frames;

        if (end_scale >= 0.0f)
        {
            p.scale = glm::mix(p.initial_scale, end_scale, lifetime_progress);
        }

        if (end_color_r >= 0)
        {
            float newR = glm::mix((float)p.initial_r, (float)end_color_r, lifetime_progress);
            p.r = (int)newR;
        }
        if (end_color_g >= 0)
        {
            float newG = glm::mix((float)p.initial_g, (float)end_color_g, lifetime_progress);
            p.g = (int)newG;
        }
        if (end_color_b >= 0)
        {
            float newB = glm::mix((float)p.initial_b, (float)end_color_b, lifetime_progress);
            p.b = (int)newB;
        }
        if (end_color_a >= 0)
        {
            float newA = glm::mix((float)p.initial_a, (float)end_color_a, lifetime_progress);
            p.a = (int)newA;
        }

        ImageSystem::DrawEx(
            p.image,
            p.x, p.y,
            p.rotation,
            p.scale, p.scale,
            0.5f, 0.5f,
            p.r, p.g, p.b, p.a,
            p.sorting_order
        );
    }
    local_frame_number++;
}

size_t ParticleSystem::CreateParticleSlot()
{
    if (!m_freeList.empty())
    {
        size_t slot = m_freeList.front();
        m_freeList.pop();
        return slot;
    }
    size_t slotIndex = m_particles.size();
    m_particles.push_back(Particle());
    return slotIndex;
}

void ParticleSystem::GenerateNewParticles(int quantity)
{
    float actorX = 0.0f;
    float actorY = 0.0f;
    if (actorOwner)
    {
        auto rbRef = actorOwner->GetComponent("Rigidbody");
        if (!rbRef.isNil() && rbRef.isUserdata())
        {
            auto* rb = rbRef.cast<Rigidbody*>();
            if (rb) {
                b2Vec2 pos = rb->GetPosition();
                actorX = pos.x;
                actorY = pos.y;
            }
        }
    }

    for (int i = 0; i < quantity; i++)
    {
        size_t slot = CreateParticleSlot();
        Particle& p = m_particles[slot];

        p.active = true;
        p.birth_frame = local_frame_number;

        float angleDeg = emit_angle_distribution.Sample();
        float angleRad = glm::radians(angleDeg);
        float radius   = emit_radius_distribution.Sample();

        float px = actorX + x + glm::cos(angleRad) * radius;
        float py = actorY + y + glm::sin(angleRad) * radius;

        float rot  = rotation_distribution.Sample();
        float sc   = scale_distribution.Sample();

        float spd  = speed_distribution.Sample();
        float rotspd = rotation_speed_distribution.Sample();

        p.x = px;
        p.y = py;
        p.rotation = rot;
        p.scale = sc;
        p.r = start_color_r;
        p.g = start_color_g;
        p.b = start_color_b;
        p.a = start_color_a;
        p.image = image.empty() ? "default_particle" : image;
        p.sorting_order = sorting_order;

        float cosA = glm::cos(angleRad);
        float sinA = glm::sin(angleRad);
        p.vx = cosA * spd;
        p.vy = sinA * spd;
        p.angular_velocity = rotspd;

        p.initial_scale = sc;
        p.initial_r = start_color_r;
        p.initial_g = start_color_g;
        p.initial_b = start_color_b;
        p.initial_a = start_color_a;
    }
}

void ParticleSystem::Stop()
{
    emission_allowed = false;
}
void ParticleSystem::Play()
{
    emission_allowed = true;
}
void ParticleSystem::Burst()
{
    GenerateNewParticles(burst_quantity);
}
