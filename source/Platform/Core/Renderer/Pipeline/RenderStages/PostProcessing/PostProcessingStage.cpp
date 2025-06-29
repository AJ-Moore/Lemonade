
#include <Platform/Core/Renderer/Pipeline/RenderStages/PostProcessing/PostProcessingStage.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/PostProcessing/Passes/FinalBlitPass.h>

#include <memory>
namespace Lemonade {

    bool PostProcessingStage::Init() 
    {
        AddPass(std::make_shared<FinalBlitPass>());
        return true;
    }

}