#pragma once

#include <LCommon.h>
#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Components/Particles/ParticleData.h>
#include <cstdint>

namespace Lemonade
{
    enum class ParticleEmissionShape : uint32_t
    {
        Point = 0,
        Sphere = 1,
        Cone = 2,
        Line = 3,
    };

    class LEMONADE_API LParticleSystem : public Lemonade::LMeshRenderer
    {
    public:
        LParticleSystem();
        virtual ~LParticleSystem() = default;

        virtual bool Init() override;
        virtual void Render() override;
        
        void SetData(const ParticleData& settings) { m_particleData = settings; }
        ParticleData& GetData() { return m_particleData; }

    private:
        void UpdateParticleDataBuffer();

        std::vector<Particle> m_particles;
        std::vector<Emitter> m_emitters;
        ParticleData m_particleData;
        std::shared_ptr<Lemonade::LUniformBuffer> m_particleDataBuffer;
        std::shared_ptr<Lemonade::LUniformBuffer> m_particlesBuffer;
        std::shared_ptr<Lemonade::LUniformBuffer> m_emitterBuffer;

        uint32 m_particleDataBufferLocation = 8;
        uint32 m_particlesBufferLocation = 9;
        uint32 m_emitterBufferLocation = 10;

        CitrusCore::Property<uint32_t> m_particleCount = 1000;
        CitrusCore::Property<ParticleEmissionShape> m_emissionShape = ParticleEmissionShape::Point;
        CitrusCore::Property<float> m_emissionAngle = 45.0f;
        CitrusCore::Property<float> m_emissionAngleVariance = 10.0f;
        CitrusCore::Property<float> m_emissionRadius = 1.0f;
        CitrusCore::Property<glm::vec4> m_emissionDirection = glm::vec4(0, 1, 0, 0);
        CitrusCore::Property<glm::vec4> m_gravity = glm::vec4(0, -0.00f, 0, 0);
        CitrusCore::Property<glm::vec4> m_initialSize = glm::vec4(1, 1, 1, 0);
        CitrusCore::Property<glm::vec4> m_initialColor = glm::vec4(1, 1, 1, 1);
        CitrusCore::Property<glm::vec4> m_initialVelocity = glm::vec4(0, 0.2, 0, 0);
        CitrusCore::Property<float> m_particleLifetime = 5.0f;
    };
}
