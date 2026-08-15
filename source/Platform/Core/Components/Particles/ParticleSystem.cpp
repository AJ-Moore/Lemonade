#include <Platform/Core/Components/Particles/ParticleData.h>
#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Core/Renderer/Geometry/PrimitiveMode.h>
#include <Platform/Core/Components/Particles/ParticleSystem.h>
#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>

namespace Lemonade
{
    bool LParticleSystem::Init()
    {
        if (!Lemonade::LMeshRenderer::Init())
        {
            return false;
        }

        int particleCount = 1000;
        int threadCount = 256;

        m_particles.resize(particleCount);
        m_emitters.resize(1);

        auto material = Lemonade::GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/particle.mat.json");
        SetMaterial(material);
        m_particleDataBuffer = std::make_shared<Lemonade::LUniformBuffer>(Lemonade::LBufferType::Uniform, &m_particleData, sizeof(ParticleData));
        m_particleDataBuffer->SetShaderStage(VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
        m_renderBlock->AddUniformBuffer(m_particleDataBuffer);

        m_particlesBuffer = std::make_shared<Lemonade::LUniformBuffer>(Lemonade::LBufferType::Storage, (void*)m_particles.data(), sizeof(Particle) * m_particles.size());
        m_particlesBuffer->SetShaderStage(VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_COMPUTE_BIT);
        m_renderBlock->AddUniformBuffer(m_particlesBuffer);

        m_emitterBuffer = std::make_shared<Lemonade::LUniformBuffer>(Lemonade::LBufferType::Storage, (void*)m_emitters.data(), sizeof(Emitter) * m_emitters.size());
        m_emitterBuffer->SetShaderStage(VK_SHADER_STAGE_COMPUTE_BIT);
        m_renderBlock->AddUniformBuffer(m_emitterBuffer);

        std::shared_ptr<std::vector<glm::vec3>> vertices = std::make_shared<std::vector<glm::vec3>>();
        vertices->resize(particleCount);

        m_mesh = std::make_shared<Lemonade::Mesh>();
        SetMesh(m_mesh);
        SetDrawMode(Lemonade::PrimitiveMode::POINTS);
        m_mesh->SetVertices(vertices);
        m_renderBlock->SetComputeGroupSize(glm::ivec3(std::ceil(static_cast<float>(particleCount)/ threadCount),1,1));

        return true;
    }

    void LParticleSystem::Render()
    {
        UpdateParticleDataBuffer();
        LMeshRenderer::Render();
    }

    void LParticleSystem::UpdateParticleDataBuffer()
    {
        LCamera* activeCamera = GraphicsServices::GetRenderer()->GetActiveCamera();

        m_particleData.Time = Lemonade::GraphicsServices::GetTime()->GetTimeElapsed();
        m_particleData.DeltaTime = Lemonade::GraphicsServices::GetTime()->GetDeltaTime();
        m_particleData.CameraForward = glm::vec4(activeCamera->GetTransform()->GetForward(), 0.0f);
        m_particleData.CameraRight = glm::vec4(activeCamera->GetTransform()->GetRight(), 0.0f);
        m_particleData.CameraUp = glm::vec4(activeCamera->GetTransform()->GetUp(), 0.0f);
        m_particleDataBuffer->SetDirty();
    }
}
