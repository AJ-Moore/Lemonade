#include <Platform/Core/Renderer/Pipeline/RenderStages/Shadows/ShadowStage.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Shadows/Passes/ShadowPass.h>
#include <memory>

namespace Lemonade {

    bool ShadowStage::Init() 
    {
        AddPass(std::make_shared<ShadowPass>());
        return true;
    }
}