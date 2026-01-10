#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/GeometryStage.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/PostProcessing/PostProcessingStage.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>
#include <memory>

namespace Lemonade {
    bool LRenderer::Init()
    {
        SetName("Renderer");
        m_renderStages.push_back(std::make_shared<GeometryStage>());
        m_renderStages.push_back(std::make_shared<PostProcessingStage>());

        for (auto& renderStage : m_renderStages)
        {
            renderStage->Init();
        }
        return true;
    }
    
    void LRenderer::Unload()
    {
        for (auto& renderStage : m_renderStages)
        {
            renderStage->Unload();
        }
    }
    
    void LRenderer::Update()
    {
        for (auto& renderStage : m_renderStages)
        {
            renderStage->Update();
        }
    }
    
    void LRenderer::Render()
    {
        // Renderer is called to render from window manager but obviously this is incompatible with Services->Render flow 
    }

    void LRenderer::RenderPass()
    {
        m_renderingData.ActiveCamera = m_activeCamera;
        m_renderingData.ActiveWindow = GraphicsServices::GetWindowManager()->GetActiveWindow();
        m_renderingData.RenderInput = m_renderInput;

        for (auto& renderStage : m_renderStages)
        {
            renderStage->Render(m_renderingData);
        }
    }

    void LRenderer::RenderScene()
    {
        OnRenderScene.Invoke(m_renderingData);
        m_renderingData.RenderInput = m_renderInput;
    }
}
