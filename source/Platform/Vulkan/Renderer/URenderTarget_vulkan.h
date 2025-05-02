#pragma once

#if defined(RENDERER_VULKAN)
#include <Renderer/UMaterial.h>
#include <Renderer/ARenderTarget.h>

namespace UpsilonEngine
{
    struct VulkanRenderTarget
    {
        VkImageView ImageView;
        VkImage Image;
    };

    class URenderTarget : public ARenderTarget
    {
    public:
        URenderTarget();
        URenderTarget(glm::ivec2 dimensions);
        virtual ~URenderTarget();
        virtual bool init();
        virtual void initAsDefault() { m_bDoneInit = true; }
        virtual void bindColourAttachments();
        virtual void bindColourAttachment(UColourAttachment colourAttachment, uint activeTarget = 0);
        virtual void bindDepthAttachment(uint activeTarget = 0);
        virtual void beginRenderPass();
        virtual void endRenderPass();

        virtual void blit(ARenderTarget& target);
        virtual void blit(unsigned int target);
        virtual void blitToScreen();
        virtual void setDimensions(glm::ivec2 dimensions);
        virtual void setColourAttachments(const std::vector<UColourAttachment> attachments, bool multiSampled);
        virtual void addMultiSampledDepthAttachment();
        virtual void addDepthAttachment(bool useRenderBufferObject = true, int layers = 1);
        virtual uint createColourAttachment(UColourAttachment colourAttachment, bool multiSampled = true, int internalFormat = U_RGBA32F);

        void setClearColour(glm::vec4 clearColour) { m_clearColour = clearColour; }
        virtual void clear(uint clearFlags);

        uint getDepthTexture() { return m_depthTexture; }


        // Gets the screen target.
        virtual ARenderTarget* GetScreenTarget();
    private:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VulkanRenderTarget getColourAttachment(UColourAttachment colourAttachment);
        VkFramebuffer getFrameBuffer() { return m_frameBuffer; }
        void generateBuffers();

        bool m_dirtyBuffer = false;
        bool m_hasMultisampledColourAttachment = false;
        uint m_depthBuffer = 0;
        uint m_depthTexture = 0;
        std::map<UColourAttachment, VulkanRenderTarget> m_colourAttachments;

        bool m_bHasDepthAttachment = false;
        VulkanRenderTarget m_depthAttachment;
        VkDeviceMemory m_depthMemory;

        VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_8_BIT;
        VkFramebuffer m_frameBuffer = 0;
        VkRenderPass m_renderPass = 0;
        VkCommandBuffer m_commandBuffer = 0;

        static UniquePtr<URenderTarget> m_defaultTarget;
    };
}

#endif