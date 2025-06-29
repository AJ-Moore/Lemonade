#pragma once

#include <Util/UID.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <LCommon.h>
#include <memory>
#include <vector>
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

    class LWindow;

    class LRenderTarget : public ARenderTarget
    {
    public:
        LRenderTarget();
        LRenderTarget(glm::ivec2 dimensions);
        virtual ~LRenderTarget();
        virtual bool Init() override;
        virtual void InitAsDefault() override;
        virtual void bindColourAttachments() override;
        virtual void bindColourAttachment(LColourAttachment colourAttachment, uint activeTarget = 0) override;
        virtual void bindDepthAttachment(uint activeTarget = 0) override;
        virtual void BeginRenderPass() override;
        virtual void EndRenderPass() override;

        virtual void blit(ARenderTarget& target) override;
        virtual void blit(unsigned int target) override;
        virtual void blitToScreen() override;
        virtual void setDimensions(glm::ivec2 dimensions) override;
        virtual void SetColourAttachments(const std::vector<LColourAttachment> attachments, bool multiSampled) override;
        virtual void SetColourAttachments(int count, bool multisampled) override;
        virtual void addMultiSampledDepthAttachment() override;
        virtual void AddDepthAttachment(bool useRenderBufferObject = true, int layers = 1) override;
        virtual uint CreateColourAttachment(LColourAttachment colourAttachment, bool multiSampled = true, int internalFormat = U_RGBA32F) override;

        virtual void Clear(uint clearFlags) override;

        uint getDepthTexture() { return m_depthTexture; }

        // TODO -> Add max frames in flight currently hardcoded to 1, move somewhere else? -> Use m_imageCount = capabilities.minImageCount + 1; 
		static const int MAX_FRAMES_IN_FLIGHT = 4;

        // Gets the screen target.
        virtual ARenderTarget* GetScreenTarget(LWindow* window) override;
        VkCommandBuffer GetCommandBuffer() const { return m_activeBuffer; }
        VkRenderPass GetRenderPass() const { return m_renderPass; }

        static LRenderTarget& GetDefault();
    private:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VulkanRenderTarget GetColourAttachment(LColourAttachment colourAttachment);
        VkFramebuffer getFrameBuffer() { return m_frameBuffer; }
        void GenerateBuffers();

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
        VkCommandBuffer m_commandBuffer[MAX_FRAMES_IN_FLIGHT];
        VkCommandBuffer m_activeBuffer;
        bool m_bRenderToScreen = false;

        VkSemaphore m_renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
        VkFence m_inFlightFence[MAX_FRAMES_IN_FLIGHT];
        VkFence m_queueFence[MAX_FRAMES_IN_FLIGHT];

        // Target per swapchain image.
        static std::unordered_map<CitrusCore::UID, std::vector<std::shared_ptr<LRenderTarget>>> m_defaultTargets;
    };
}

#endif