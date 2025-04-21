#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <string>
#include <vector>
#include <queue>
#include "Actor.h"
#include "Helper.h"

struct Particle
{
    bool active = false;
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;
    float scale = 1.0f;
    int r = 255;
    int g = 255;
    int b = 255;
    int a = 255;
    std::string image = "default_particle";
    int sorting_order = 9999;
    float vx = 0.0f;
    float vy = 0.0f;
    float angular_velocity = 0.0f;
    int birth_frame = 0;
    float initial_scale = 1.0f;
    int initial_r = 255;
    int initial_g = 255;
    int initial_b = 255;
    int initial_a = 255;
};

class ParticleSystem
{
public:
    bool enabled = true;
    std::string key;
    bool __has_started = false;
    std::string __script_type = "ParticleSystem";

    Actor* actorOwner = nullptr;

    RandomEngine emit_angle_distribution;
    RandomEngine emit_radius_distribution;

    float x = 0.0f;
    float y = 0.0f;

    float emit_angle_min = 0.0f;
    float emit_angle_max = 360.0f;
    float emit_radius_min = 0.0f;
    float emit_radius_max = 0.5f;

    int frames_between_bursts = 1;
    int burst_quantity = 1;

    float start_scale_min = 1.0f;
    float start_scale_max = 1.0f;
    float rotation_min = 0.0f;
    float rotation_max = 0.0f;

    int start_color_r = 255;
    int start_color_g = 255;
    int start_color_b = 255;
    int start_color_a = 255;

    std::string image = "";
    int sorting_order = 9999;

    RandomEngine rotation_distribution;
    RandomEngine scale_distribution;

    int duration_frames = 300;
    float start_speed_min = 0.0f;
    float start_speed_max = 0.0f;
    float rotation_speed_min = 0.0f;
    float rotation_speed_max = 0.0f;
    float gravity_scale_x = 0.0f;
    float gravity_scale_y = 0.0f;
    float drag_factor = 1.0f;
    float angular_drag_factor = 1.0f;
    float end_scale = -1.0f;
    int end_color_r = -1;
    int end_color_g = -1;
    int end_color_b = -1;
    int end_color_a = -1;

    RandomEngine speed_distribution;
    RandomEngine rotation_speed_distribution;

    int local_frame_number = 0;

    std::vector<Particle> m_particles;
    std::queue<size_t> m_freeList;

    bool emission_allowed = true;

    ParticleSystem() = default;
    ParticleSystem(const ParticleSystem& other);

    void OnStart();
    void OnUpdate();

    void Stop();
    void Play();
    void Burst();
private:
    size_t CreateParticleSlot();
    void GenerateNewParticles(int quantity);
};

#endif
