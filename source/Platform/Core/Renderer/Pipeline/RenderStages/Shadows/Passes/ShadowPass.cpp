#include <Platform/Core/Renderer/Lighting/Light.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Shadows/Passes/ShadowPass.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>

namespace Lemonade
{
    ShadowPass::ShadowPass() : m_shadows( glm::ivec2(GraphicsServices::GetRenderer()->GetShadowMapSize()), m_maxShadowMaps, false)
    {
    }

    bool ShadowPass::Init()
    {
        m_shadows.Init();
        // Should match geometry target in geometry pass
        m_shadows.SetColourAttachments(4, false);
        m_shadows.AddDepthAttachment();
        return true;
    }

    void ShadowPass::Render( const LRenderingData& renderingData)
    {
        ARenderTarget* previousTarget = GraphicsServices::GetRenderer()->GetActiveRenderTarget();
        GraphicsServices::GetRenderer()->SetShadowPass(true);

        for (int i = 0; i < renderingData.RenderInput->LightData.Count || i < m_maxShadowMaps; ++i)
        {
            const Lemonade::LightingData* light = &renderingData.RenderInput->LightData.LightPtr[i];
            glm::vec3 position = light->LightPosition;
            glm::mat4 view = glm::lookAtLH(position, position + light->LightDirection, light->LightUp);
            glm::mat4 projection = glm::perspectiveLH(glm::pi<float>() * 0.5f, 1.0f, 0.1f,1000.0f);

            GraphicsServices::GetRenderer()->SetShadowData({
                .View = view, 
                .Projection = projection
            });
            m_shadows.BeginRenderPass(i);
            GraphicsServices::GetRenderer()->RenderScene();
            m_shadows.EndRenderPass();
        }

        GraphicsServices::GetRenderer()->SetShadowPass(false);

        // Reset render target
        GraphicsServices::GetRenderer()->SetActiveRenderTarget(previousTarget);
    }

    void ShadowPass::Update()
    {
    }
}