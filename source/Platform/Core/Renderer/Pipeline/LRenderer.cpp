#include "LRenderer.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade {
    bool LRenderer::Init()
    {
        SetName("Renderer");
        return true;
    }
    
    void LRenderer::Unload()
    {
    }
    
    void LRenderer::Update()
    {
    }
    
    void LRenderer::Render()
    {
        for (auto& renderStage : m_renderStages)
        {
            renderStage->Render(m_renderingData);
        }
    }

    void LRenderer::RenderScene()
    {
        OnRenderScene.Invoke(m_renderingData);
    }
}

