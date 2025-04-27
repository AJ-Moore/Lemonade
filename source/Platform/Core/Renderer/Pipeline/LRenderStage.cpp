#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade
{
    void LRenderStage::AddPass(std::shared_ptr<LRenderPass> renderPass)
    {
        m_renderPasses.push_back(renderPass);
    }
}