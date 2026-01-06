
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>

namespace Lemonade
{
    bool ARenderBlock::AddUniformBuffer(std::shared_ptr<AUniformBuffer> buffer)
    {
        m_uniformBuffers.Add(buffer->GetUID(), buffer);
    }

    bool ARenderBlock::RemoveUniformBuffer(CitrusCore::UID uniformBufferuid)
    {
        if (!m_uniformBuffers.Contains(uniformBufferuid))
        {
            return false;
        }

        m_uniformBuffers.Remove(uniformBufferuid);
        return true;
    }
}