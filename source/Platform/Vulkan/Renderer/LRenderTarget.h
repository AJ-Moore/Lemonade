#pragma once

#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>
#include <Util/UID.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <LCommon.h>
#include <cstdint>
#include <memory>
#include <unordered_map>
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
        VkDeviceMemory Memory;
    };

    class LWindow;

    class LRenderTarget : public ARenderTarget
    {
        struct ImageTransition
        {
            VkImageLayout oldLayout;
            VkImageLayout newLayout;
            VkPipelineStageFlags srcStage;
            VkPipelineStageFlags dstStage;
            VkAccessFlags srcAccess;
            VkAccessFlags dstAccess;
            VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        };
    public:
        LRenderTarget();
        LRenderTarget(glm::ivec2 dimensions);
        virtual ~LRenderTarget();
        virtual bool Init() override;
        virtual void InitAsDefault() override;
        virtual void BindColourAttachments() override;
        virtual void BindColourAttachment(LColourAttachment colourAttachment, uint activeTarget = 0) override;
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
        VkSampleCountFlagBits GetSampleCount() const { return m_hasMultisampledColourAttachment ? m_sampleCount : VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT; }
        uint32_t GetAttachmentCount() const { return m_colourAttachments.size(); }
        virtual void SetRenderBlock(ARenderBlock* renderBlock) override; 

        static LRenderTarget& GetDefault();
    private:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VulkanRenderTarget GetColourAttachment(LColourAttachment colourAttachment);
        VkFramebuffer getFrameBuffer() { return m_frameBuffer; }
        void GenerateBuffers();
        void UpdateDescriptorSets();
        void TransitionAttachments(VkCommandBuffer cmdBuffer, ImageTransition transition);
        void CreateDescriptorSetLayout();

        bool m_dirtyBuffer = false;
        bool m_descriptorsDirty = true;
        bool m_hasMultisampledColourAttachment = false;
        uint m_depthBuffer = 0;
        uint m_depthTexture = 0;
        std::map<LColourAttachment, VulkanRenderTarget> m_colourAttachments;

        VkDescriptorSet m_descriptorSet = nullptr;
        VkDescriptorSetLayout m_descriptorSetLayout;
        std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

        bool m_bHasDepthAttachment = false;
        VulkanRenderTarget m_depthAttachment;
        VkDeviceMemory m_depthMemory;

        VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkFramebuffer m_frameBuffer = 0;
        VkRenderPass m_renderPass = 0;
        VkCommandBuffer m_commandBuffer[MAX_FRAMES_IN_FLIGHT];
        VkCommandBuffer m_activeBuffer;
        bool m_bRenderToScreen = false;

        VkDescriptorPool m_descriptorPool;
        VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;

        VkSemaphore m_renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
        VkFence m_inFlightFence[MAX_FRAMES_IN_FLIGHT];
        VkFence m_queueFence[MAX_FRAMES_IN_FLIGHT];

        // Target per swapchain image.
        static std::unordered_map<CitrusCore::UID, std::vector<std::shared_ptr<LRenderTarget>>> m_defaultTargets;

        VkSampler m_linearSampler = VK_NULL_HANDLE;

        const ImageTransition ToColourAttachment{
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        };

        const ImageTransition ToShaderRead{
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT
        };
    };
}

#endif