#pragma once
#include <LCommon.h>

#ifdef RENDERER_VULKAN

#include <Platform/Core/Renderer/Pipeline/AViewport.h>

namespace Lemonade 
{
    class LEMONADE_API LViewport : public AViewport
    {
    public:
        /// Apply the scissor/ viewport 
        void VulkanApply(VkCommandBuffer commandBuffer);
    protected:
        virtual void SetViewport(int x, int y, int width, int height);
        virtual void SetScissor(int x, int y, int width, int height); 
        virtual void SetDepthRange(float near, float far);
    private:
        VkRect2D m_scissor{};
        VkViewport m_viewport{};
    };
}

#endif 