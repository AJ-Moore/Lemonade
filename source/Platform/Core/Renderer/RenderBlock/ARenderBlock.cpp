
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>

#ifdef RENDERER_VULKAN 
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#endif

namespace Lemonade
{
    bool ARenderBlock::AddUniformBuffer(std::shared_ptr<LUniformBuffer> buffer)
    {
        if (m_uniformBuffers.Contains(buffer->GetUID()))
        {
            return false;
        }

        m_uniformBuffers.Add(buffer->GetUID(), buffer);
        buffer->Init();
        return true;
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

    bool ARenderBlock::AddBinding(std::shared_ptr<LBinding> binding)
    {
        if (m_additionalBindings.Contains(binding->GetUID()))
        {
            return false;
        }

        m_additionalBindings.Add(binding->GetUID(), binding);
        binding->Init();
        return true;
    }

    bool ARenderBlock::RemoveBinding(CitrusCore::UID bindinguid)
    {
        if (!m_additionalBindings.Contains(bindinguid))
        {
            return false;
        }

        m_additionalBindings.Remove(bindinguid);
        return true;
    }
}