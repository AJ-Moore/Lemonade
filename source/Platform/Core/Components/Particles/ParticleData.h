#pragma once
#include <LCommon.h>
#include <cstdint>
#include <glm/fwd.hpp>

namespace Lemonade 
{
    struct alignas(16) ParticleData
    {
        float SpawnRate = 1000.0f;
        float Lifetime = 1.0f;
        uint32_t MaxParticles = 1000; // should match number of points in particle system
        uint32_t EmissionShape = 0;
        float EmissionAngle = 45.0f;
        float EmissionAngleVariance = 10.0f;
        float EmissionRadius = 1.0f;
        float Time = 0.0f;
        float DeltaTime = 0.016f;
        glm::vec3 padding0;
        glm::vec4 EmissionDirection = { 0, 1, 0, 0 };
        glm::vec4 Gravity = { 0, -0.00f, 0, 0 };
        glm::vec4 InitialVelocity = { 0, 0.2, 0, 0 };
        glm::vec4 InitialSize = { 0.1, 0.1, 0.1, 0 };
        glm::vec4 InitialColor = { 1, 1, 1, 0 };
        glm::vec4 CameraForward = { 0, 0, 1, 0 };
        glm::vec4 CameraRight = { 1, 0, 0, 0 };
        glm::vec4 CameraUp = { 0, 1, 0, 0 };
    };

    struct alignas(16) Particle
    {
        glm::vec4 Position;
        glm::vec4 Velocity;
        float Age = 0;
        uint Alive = 0;
        glm::vec2 padding0;
    };

    struct alignas(16) Emitter
    {
        uint LastSpawn = 0;
    };
}