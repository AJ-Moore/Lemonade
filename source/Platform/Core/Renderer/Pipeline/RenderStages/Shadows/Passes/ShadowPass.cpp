#include <Platform/Core/Renderer/Lighting/Light.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Shadows/Passes/ShadowPass.h>
#include <glm/ext/quaternion_geometric.hpp>
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

    void ShadowPass::RenderShadow(const glm::mat4& view, const glm::mat4& projection, int layer)
    {
        GraphicsServices::GetRenderer()->SetShadowData({
            .View = view, 
            .Projection = projection
        });
        m_shadows.BeginRenderPass(layer);
        GraphicsServices::GetRenderer()->RenderScene();
        m_shadows.EndRenderPass();
    }

    void ShadowPass::Render( const LRenderingData& renderingData)
    {
        // Update lighting/ environment data etc... 
        GraphicsServices::GetRenderer()->PrepareScene();

        ARenderTarget* previousTarget = GraphicsServices::GetRenderer()->GetActiveRenderTarget();
        GraphicsServices::GetRenderer()->SetShadowPass(true);

        for (int i = 0; i < renderingData.RenderInput->LightData.Count && i < m_maxShadowMaps;)
        {
            const Lemonade::LightingData* light = &renderingData.RenderInput->LightData.LightPtr[i];

            glm::mat4 projection = glm::perspectiveLH(glm::pi<float>() * 0.5f, 1.0f, 0.1f,1000.0f);
            glm::vec3 position = light->LightPosition;

            if (light->LightType == (int)LightType::Point)
            {
                if (i + 6 >= m_maxShadowMaps)
                {
                    //Not enough remaining shadow maps.
                    continue;
                }

                glm::vec3 right = glm::cross(light->LightDirection, light->LightDirection);

                std::vector<glm::mat4> views = {
                    // Forward
                    glm::lookAtLH(position, position + light->LightDirection, light->LightUp),
                    // Back
                    glm::lookAtLH(position, position - light->LightDirection, light->LightUp),
                    // Right
                    glm::lookAtLH(position, position + right, light->LightUp),
                    // Left 
                    glm::lookAtLH(position, position - right, light->LightUp), 
                    // Up 
                    glm::lookAtLH(position, position + light->LightUp, light->LightDirection),
                    // Down 
                    glm::lookAtLH(position, position - light->LightUp, -light->LightDirection),     
                };

                for (const auto& view : views)
                {
                    RenderShadow(view, projection, i++); 
                }
 
            }
            else {
                glm::mat4 view = glm::lookAtLH(position, position + light->LightDirection, light->LightUp);
                RenderShadow(view, projection, i++); 
            }

        }

        GraphicsServices::GetRenderer()->SetShadowPass(false);

        // Reset render target
        GraphicsServices::GetRenderer()->SetActiveRenderTarget(previousTarget);
    }

    void ShadowPass::Update()
    {
    }
}