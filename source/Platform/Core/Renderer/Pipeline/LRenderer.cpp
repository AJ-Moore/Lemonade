#include "LRenderer.h"
#include "Platform/Core/Services/GraphicsServices.h"

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
}

