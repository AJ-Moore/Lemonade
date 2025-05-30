#include "Platform/Core/Services/GraphicsServices.h"
#include <LCommon.h>
#include <vulkan/vulkan_core.h>

#ifdef RENDERER_VULKAN
#include <Platform/Vulkan/Renderer/Pipeline/LViewport.h>

namespace Lemonade 
{
    void LViewport::SetViewport(int x, int y, int width, int height)
    {
        m_viewport.x = x;
        m_viewport.y = y;
        m_viewport.width = static_cast<float>(width);
        m_viewport.height = static_cast<float>(height);
        m_viewport.minDepth = 0.0f;
        m_viewport.maxDepth = 1.0f;
    }

    void LViewport::SetScissor(int x, int y, int width, int height)
    {
        // Get active command buffer.
        m_scissor.offset = {x,y};
        m_scissor.extent = {(uint)width, (uint)height};
    }

    void LViewport::VulkanApply(VkCommandBuffer commandBuffer)
    {
        vkCmdSetScissor(commandBuffer, 0, 1, &m_scissor);
        vkCmdSetViewport(commandBuffer, 0, 1, &m_viewport);
    }

    void LViewport::SetDepthRange(float near, float far)
    {
        // Get active command buffer.


        
    }
}
#endif 