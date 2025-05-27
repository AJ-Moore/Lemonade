#pragma once

#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <LCommon.h>
#include <vulkan/vulkan_core.h>
#if defined(RENDERER_VULKAN)
#include <Platform/Core/Renderer/Materials/Material.h>
#include <map>

namespace Lemonade
{
    struct VulkanRenderTarget
    {
        VkImageView ImageView;
        VkImage Image;
    };

    class LRenderTarget : public ARenderTarget
    {
    public:
        LRenderTarget();
        LRenderTarget(glm::ivec2 dimensions);
        virtual ~LRenderTarget();
        virtual bool Init();
        virtual void initAsDefault() { m_bDoneInit = true; }
        virtual void bindColourAttachments();
        virtual void bindColourAttachment(LColourAttachment colourAttachment, uint activeTarget = 0);
        virtual void bindDepthAttachment(uint activeTarget = 0);
        virtual void BeginRenderPass();
        virtual void EndRenderPass();

        virtual void blit(ARenderTarget& target);
        virtual void blit(unsigned int target);
        virtual void blitToScreen();
        virtual void setDimensions(glm::ivec2 dimensions);
        virtual void SetColourAttachments(const std::vector<LColourAttachment> attachments, bool multiSampled);
        virtual void SetColourAttachments(int count, bool multisampled);
        virtual void addMultiSampledDepthAttachment();
        virtual void AddDepthAttachment(bool useRenderBufferObject = true, int layers = 1);
        virtual uint createColourAttachment(LColourAttachment colourAttachment, bool multiSampled = true, int internalFormat = U_RGBA32F);

        void setClearColour(glm::vec4 clearColour) { m_clearColour = clearColour; }
        virtual void Clear(uint clearFlags);

        uint getDepthTexture() { return m_depthTexture; }


        // Gets the screen target.
        virtual LRenderTarget* GetScreenTarget();
        VkCommandBuffer GetCommandBuffer() const { return m_commandBuffer; }
    private:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VulkanRenderTarget getColourAttachment(LColourAttachment colourAttachment);
        VkFramebuffer getFrameBuffer() { return m_frameBuffer; }
        void generateBuffers();

        bool m_dirtyBuffer = false;
        bool m_hasMultisampledColourAttachment = false;
        uint m_depthBuffer = 0;
        uint m_depthTexture = 0;
        std::map<LColourAttachment, VulkanRenderTarget> m_colourAttachments;

        bool m_bHasDepthAttachment = false;
        VulkanRenderTarget m_depthAttachment;
        VkDeviceMemory m_depthMemory;

        VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_8_BIT;
        VkFramebuffer m_frameBuffer = 0;
        VkRenderPass m_renderPass = 0;
        VkCommandBuffer m_commandBuffer = 0;

        static std::unique_ptr<LRenderTarget> m_defaultTarget;
    };
}

#endif