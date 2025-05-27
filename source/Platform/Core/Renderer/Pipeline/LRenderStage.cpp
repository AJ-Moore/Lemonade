#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade
{
    void LRenderStage::AddPass(std::shared_ptr<ARenderPass> renderPass)
    {
        m_renderPasses.push_back(renderPass);
    }

    void LRenderStage::Render(const LRenderingData& renderingData)
    {
        for (auto& renderPass : m_renderPasses)
        {
            renderPass->Render(renderingData);
        }
    }
}