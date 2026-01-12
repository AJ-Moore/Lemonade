#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Core/Renderer/Pipeline/AUniformBuffer.h>
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/Passes/GeometryPass.h>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Lemonade
{
    GeometryPass::GeometryPass() : 
        m_deferredPass(GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/deferred.mat.json"))
    {

    }

    bool GeometryPass::Init() 
    {
        m_deferredPass.Init();
        m_geometryTarget.Init();
        m_gBuffer.Init();
        m_gBuffer.SetColourAttachments(1, false);
		m_geometryTarget.SetColourAttachments(4, false);
        m_geometryTarget.AddDepthAttachment();
        m_deferredBuffer = std::make_shared<LUniformBuffer>(LBufferType::Uniform, &m_deferredData, sizeof(DeferredData));
        m_deferredBuffer->SetShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT);
        m_deferredPass.GetRenderBlock()->AddUniformBuffer(m_deferredBuffer);
        return true;
    }

    void GeometryPass::UpdateDeferredData(const LRenderingData& renderingData)
    {
        // Lighting data per scene, not updated until after scene is rendered... 
        if (m_lightignBuffer == nullptr)
        {
            m_lightignBuffer = renderingData.RenderInput->LightBuffer;
            m_deferredPass.GetRenderBlock()->AddUniformBuffer(m_lightignBuffer);
        }

        if (m_deferredData.LightCount != renderingData.RenderInput->LightData.Count)
        {
            m_deferredData.LightCount = renderingData.RenderInput->LightData.Count;
            m_deferredBuffer->SetDirty();
        }
    }

    void GeometryPass::Render(const LRenderingData& renderingData)
    {
        m_geometryTarget.BeginRenderPass();
        m_geometryTarget.setClearColour(glm::vec4(0,0,0, 0));
        m_geometryTarget.Clear((uint)LBufferBit::COLOUR);
        GraphicsServices::GetRenderer()->RenderScene();
        m_geometryTarget.EndRenderPass();

        /// Should be called after rendering scene.
        UpdateDeferredData(renderingData);
        
        m_gBuffer.BeginRenderPass();
        m_gBuffer.setClearColour(glm::vec4(0,0,0, 0));
        m_gBuffer.Clear((uint)LBufferBit::COLOUR);
        m_deferredPass.SetRenderTarget(&m_geometryTarget);
        m_deferredPass.Render();
        m_gBuffer.EndRenderPass();
    }

    void GeometryPass::Update() 
    {
        m_deferredPass.Update();
    }
}