#include <LCommon.h>
#include <vulkan/vulkan_core.h>

#ifdef RENDERER_VULKAN
#include <Platform/Vulkan/Renderer/Pipeline/LViewport.h>

namespace Lemonade 
{
    void LViewport::SetViewport(int x, int y, int width, int height)
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    void LViewport::SetScissor(int x, int y, int width, int height)
    {
        // Get active command buffer.

        VkRect2D scissor{};
        scissor.offset = {x,y};
        scissor.extent = {(uint)width, (uint)height};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void LViewport::SetDepthRange(float near, float far)
    {
        // Get active command buffer.


        
    }
}
#endif 