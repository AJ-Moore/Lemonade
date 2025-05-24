#include "LRenderer.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include "Platform/Core/WindowManager/LWindowManager.h"
#include <memory>

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
        // Iterate through all windows.
        std::shared_ptr<LWindowManager> windowManager = GraphicsServices::GetWindowManager();

    }
}

