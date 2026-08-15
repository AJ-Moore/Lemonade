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
        LParticleSystem() = default;
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
    };
}
