#include <LCommon.h>

#ifdef RENDERER_VULKAN

#include <Platform/Vulkan/Renderer/Pipeline/LViewport.h>
#include <Platform/Vulkan/Materials/LSampler.h>
#include <utility>
#include <Platform/Vulkan/Renderer/LRenderBlock.h>
#include <Platform/Vulkan/Materials/Texture.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Util/Logger.h>
#include <climits>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <memory>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>

namespace Lemonade
{
    using CitrusCore::Logger;

    std::unordered_map<CitrusCore::UID, std::vector<std::shared_ptr<LRenderTarget>>> LRenderTarget::m_defaultTargets;

    // TODO remove this hack, thanks, No
    LRenderTarget& LRenderTarget::GetDefault()
    {
        static LRenderTarget target; 
        return target;
    }

    LRenderTarget::LRenderTarget()
    {
        auto window = GraphicsServices::GetWindowManager()->GetMainWindow();

        if (window != nullptr)
        {
            // Default to window dimensions? SURE
            m_colourDimensions = glm::ivec2(window->GetWidth(), window->GetHeight());
            m_depthDimensions = m_colourDimensions;
        }
    }

    LRenderTarget::LRenderTarget(glm::ivec2 dimensions, uint32 layerCount, bool arrayTexture) : LRenderTarget(dimensions,dimensions, layerCount, arrayTexture){
    }

    LRenderTarget::LRenderTarget(glm::ivec2 colourDimensions, glm::ivec2 depthDimensions, uint32 layerCount, bool arrayTexture) : ARenderTarget(colourDimensions, depthDimensions, layerCount, arrayTexture){
    }


    LRenderTarget::~LRenderTarget()
    {
        if (m_frameBuffer.size())
        {
            VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
            for (auto& framebuffer : m_frameBuffer)
            {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
        }
    }

    void LRenderTarget::InitAsDefault()
    {
        m_bRenderToScreen = true;
    }

    bool LRenderTarget::Init()
    {
        if (m_bDoneInit)
        {
            return true;
        }

        m_bDoneInit = true;
        m_dirtyBuffer = true;

        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();


        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
        {
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]);
    
            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = 0;

            if (vkCreateFence(device, &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create in-flight fence!");
            }

            if (vkCreateFence(device, &fenceInfo, nullptr, &m_queueFence[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create in-flight fence!");
            }
        }

		VkDescriptorPoolSize poolSize[] = {
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = 200
            },
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = 100
            },
			{
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 100
            }
        };

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 100;
        poolInfo.poolSizeCount = 3;
        poolInfo.pPoolSizes = poolSize;

        vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool);

        return true;
    }

    void LRenderTarget::BindColourAttachments()
    {
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

        LWindow* activeWindow = GraphicsServices::GetWindowManager()->GetActiveWindow();
        uint32_t currentFrame = activeWindow->GetCurrentFrame();
        LRenderTarget* activeTarget = static_cast<LRenderTarget*>(GraphicsServices::GetRenderer()->GetActiveRenderTarget());
        LRenderBlock* renderBlock = static_cast<LRenderBlock*>(m_renderBlock);
        VkDescriptorSet descriptorSet = renderBlock->GetDescriptorSet(currentFrame);

        // see m_bTextureSamplersDirty in LRenderBlock.cpp, bug currently prevents this optimisation.
        //if (m_descriptorsDirty)
        {
            UpdateDescriptorSets(descriptorSet);
        }

        m_descriptorsDirty = false;
    }

    void LRenderTarget::BindDepthAttachment(uint32 bindIndex)
    {
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        LRenderBlock* renderBlock = static_cast<LRenderBlock*>(m_renderBlock);

        bool useCombinedImageSampler = m_bArrayTexture ? false : true;
        std::vector<VkDescriptorImageInfo> imageDescriptors;
        int layers = m_bArrayTexture ? 1 : m_layerCount;


		LSampler* defaultSampler = static_cast<LSampler*>(renderBlock->GetMaterial()->GetResource()->GetSamplers().begin()->get());

        for (int i = 0; i < layers; ++i)
        {
            VkDescriptorImageInfo imageDescriptor = {};
            imageDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageDescriptor.imageView = m_depthAttachment.ImageViews[i]; /// This should be the index of the layer we want to bind when not using array texture?
            //imageDescriptor.sampler = defaultSampler->GetSampler();
            imageDescriptors.push_back(std::move(imageDescriptor));
        }

        LWindow* activeWindow = GraphicsServices::GetWindowManager()->GetActiveWindow();
        uint32_t currentFrame = activeWindow->GetCurrentFrame();
        VkDescriptorSet descriptorSet = renderBlock->GetDescriptorSet(currentFrame);

        std::vector<VkWriteDescriptorSet> writes;
        VkWriteDescriptorSet writeImage = {};
        writeImage.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeImage.dstSet = descriptorSet;
        writeImage.dstBinding = bindIndex;
        writeImage.dstArrayElement = 0;
        // I think this was probably a mistake, I'm not sure what I was thinking here?
        //writeImage.descriptorType = m_bArrayTexture ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeImage.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writeImage.descriptorCount = m_bArrayTexture ? 1 : m_layerCount;
        writeImage.pImageInfo = imageDescriptors.data();
        writes.push_back(writeImage);
        
        vkUpdateDescriptorSets(device, writes.size(), writes.data(), 0, nullptr);
    }

    void LRenderTarget::GenerateBuffers()
    {
        // Generate Vulkan Framebuffer
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

        // Destroy framebuffer if it exists
        if (m_frameBuffer.size())
        {
            for (auto& framebuffer : m_frameBuffer)
            {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
        }

        // Destroy render pass if it exists
        if (m_renderPass)
        {
            vkDestroyRenderPass(device, m_renderPass, nullptr);
        }

        std::vector<VkAttachmentReference> attachmentRefs;
        std::vector<VkAttachmentDescription> attachmentDescription;
        int attachmentCount = 0;

        for (int i = 0; i < m_colourAttachments.size(); ++i)
        {
            VkAttachmentDescription colorAttachment = {};
            colorAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
            colorAttachment.samples = m_hasMultisampledColourAttachment ? m_sampleCount : VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            if (m_bRenderToScreen)
            {
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }

            attachmentDescription.push_back(colorAttachment);

            VkAttachmentReference attachmentRef = {};
            attachmentRef.attachment = i;
			attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentRefs.push_back(attachmentRef);
            attachmentCount++;
		}

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = m_colourAttachments.size();;
        subpass.pColorAttachments = attachmentRefs.data();

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = attachmentCount++;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        if (m_bHasDepthAttachment)
        {
            subpass.pDepthStencilAttachment = &depthAttachmentRef;

            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = VK_FORMAT_D32_SFLOAT;
            depthAttachment.samples = m_hasMultisampledColourAttachment ? m_sampleCount : VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachmentDescription.push_back(depthAttachment);
        }

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachmentDescription.size();
        renderPassInfo.pAttachments = attachmentDescription.data();
        renderPassInfo.subpassCount = 1; 
        renderPassInfo.pSubpasses = &subpass; 
        VkSubpassDependency dependency = {};

        if (m_bRenderToScreen)
        {
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;
        }

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

        uint32 imageViewCount = m_bArrayTexture ? 1 : m_layerCount;
        m_frameBuffer.resize(imageViewCount);

        for (int i = 0; i < imageViewCount; ++i)
        {
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = attachmentDescription.size();
            framebufferInfo.width = std::max(m_colourDimensions.x, m_depthDimensions.x);
            framebufferInfo.height = std::max(m_colourDimensions.y, m_depthDimensions.y);
            framebufferInfo.layers = 1;
    
            std::vector<VkImageView> images;
    
            for (int p = 0; p < m_colourAttachments.size(); ++p)
            {
                const auto& attachment = m_colourAttachments.at(static_cast<LColourAttachment>(((uint)LColourAttachment::LEMON_COLOR_ATTACHMENT0 + p)));
                images.push_back(attachment.ImageViews[i]);
            }
    
            if (m_bHasDepthAttachment)
            {
                images.push_back(m_depthAttachment.ImageViews[i]);
            }
    
            framebufferInfo.pAttachments = images.data();
            m_frameBuffer[i] = {};
    
            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffer[i]) != VK_SUCCESS)
            {
                Logger::Log(Logger::ERROR,"Failed to create framebuffer!");
                throw std::runtime_error("Failed to create framebuffer!");
            }
        } 

        // Create command buffer
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = GraphicsServices::GetContext()->GetVulkanDevice().GetGraphicsCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
        {
            if (vkAllocateCommandBuffers(device, &allocInfo, &m_commandBuffer[i]) != VK_SUCCESS)
            {
                Logger::Log(Logger::ERROR, "Failed to allocate command buffer!");
                throw std::runtime_error("Failed to allocate command buffers!");
            }
        }

		m_dirtyBuffer = false;
    }

    void LRenderTarget::BeginRenderPass(uint32 layerIndex)
    {
        // If buffer dirty or not generated, generate it. Step was to reduce uneccessary buffer re-generation when adding colour targets & such.
        if (m_dirtyBuffer)
        {
            GenerateBuffers();
        }

        LWindow* activeWindow = GraphicsServices::GetWindowManager()->GetActiveWindow();
        uint32_t currentFrame = activeWindow->GetCurrentFrame();

        GraphicsServices::GetRenderer()->SetActiveRenderTarget(this);
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

        std::vector<VkClearValue> clearValues;
        clearValues.resize(m_colourAttachments.size() + ((m_bHasDepthAttachment) ? 1 : 0));
        int c = 0;

        for (auto& colourAttach : m_colourAttachments)
        {
            clearValues[c++].color = { { m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a } };
        }

        if (m_bHasDepthAttachment)
        {
            clearValues[c++].depthStencil = {1.0f, 0};   
        }

        m_activeBuffer = m_commandBuffer[currentFrame];

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_frameBuffer[layerIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent.width = std::max(m_colourDimensions.x, m_depthDimensions.x);
        renderPassInfo.renderArea.extent.height = std::max(m_colourDimensions.y, m_depthDimensions.y);
        renderPassInfo.pClearValues = clearValues.data();
        renderPassInfo.clearValueCount = clearValues.size();

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        //beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Set command buffer usage flags 
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Set command buffer usage flags 
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkResetCommandBuffer(m_commandBuffer[currentFrame], 0); 
        
        VkResult result = vkBeginCommandBuffer(m_commandBuffer[currentFrame], &beginInfo);

        TransitionAttachments(m_commandBuffer[currentFrame], ToColourAttachment);

        // Upload pending texture data to GPU
        Texture::UploadTextures(m_commandBuffer[currentFrame]);
        
        if (result != VK_SUCCESS)
        {
            Logger::Log(Logger::ERROR, "Begin command buffer failed.");
        }

        // Manual trans
        if (m_bRenderToScreen)
        {
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // current assumed
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = m_colourAttachments[LColourAttachment::LEMON_COLOR_ATTACHMENT0].Image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            
            vkCmdPipelineBarrier(
                m_commandBuffer[currentFrame],
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );
        }

        
        vkCmdBeginRenderPass(m_commandBuffer[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        static_cast<LViewport*>(GraphicsServices::GetRenderer()->GetViewport())->VulkanApply(m_commandBuffer[currentFrame]);
    }

    void LRenderTarget::TransitionAttachments(VkCommandBuffer cmdBuffer, ImageTransition transition)
    {
        if (m_bRenderToScreen)
        {
            // Not valid to transition swap chain target.
            return;
        }

        std::vector<VkImageMemoryBarrier> barriers;
        for (int i = 0; i < m_colourAttachments.size(); ++i)
        {
            const auto& attachment = m_colourAttachments.at(static_cast<LColourAttachment>(((uint)LColourAttachment::LEMON_COLOR_ATTACHMENT0 + i)));
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = transition.oldLayout;
            barrier.newLayout = transition.newLayout;
            barrier.srcAccessMask = transition.srcAccess;
            barrier.dstAccessMask = transition.dstAccess;
            barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS };
            barrier.image = attachment.Image;;
            barriers.push_back(barrier);
        }

        vkCmdPipelineBarrier(
            cmdBuffer,
            transition.srcStage,
            transition.dstStage,
            0, 0, nullptr, 0, nullptr, barriers.size(), barriers.data()
        );
    }

    void LRenderTarget::EndRenderPass()
    {
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        VkResult result;

        LWindow* activeWindow = GraphicsServices::GetWindowManager()->GetActiveWindow();
        uint32_t currentFrame = activeWindow->GetCurrentFrame();
        
        vkCmdEndRenderPass(m_commandBuffer[currentFrame]);

        TransitionAttachments(m_commandBuffer[currentFrame], ToShaderRead);

        VkResult endResult = vkEndCommandBuffer(m_commandBuffer[currentFrame]);
        
        if (endResult != VK_SUCCESS) {
            Logger::Log(Logger::ERROR, "Failed to end command buffer: %d", endResult);
            return; // don't submit if ending failed
        }
        
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };        
        
        VkSemaphore semaphore = activeWindow->GetSemaphore();
        VkSemaphore timelineSemaphore = activeWindow->GetTimelineSemaphore();

        uint64_t value = activeWindow->GetFrameSemaphoreTimelineValueAndIncrement();
        uint64_t nextValue = value + 1;

        VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore[currentFrame], timelineSemaphore };
        uint64_t signalValues[] = {0,value + 1 }; 
        uint64_t waitValues[] = {0,value  }; 

        VkTimelineSemaphoreSubmitInfo timelineInfo{};

        timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        timelineInfo.signalSemaphoreValueCount = 2;

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        if (activeWindow->GetPassIndexAndIncrement() == 0)
        {
            // No wait on timeline semaphore, waits for frame aquired image semaphore
            timelineInfo.pWaitSemaphoreValues = waitValues;
            timelineInfo.waitSemaphoreValueCount = 1;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &semaphore;
        }
        else 
        {
            timelineInfo.pWaitSemaphoreValues = &waitValues[1];
            timelineInfo.waitSemaphoreValueCount = 1;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &timelineSemaphore;
        }

        if (m_bRenderToScreen)
        {
            timelineInfo.signalSemaphoreValueCount = 2;
            submitInfo.signalSemaphoreCount = 2;
            submitInfo.pSignalSemaphores = signalSemaphores;
            timelineInfo.pSignalSemaphoreValues = signalValues;
        }
        else {
            timelineInfo.signalSemaphoreValueCount = 1;
            timelineInfo.pSignalSemaphoreValues = &signalValues[1];
            submitInfo.signalSemaphoreCount = 1; 
            submitInfo.pSignalSemaphores = &timelineSemaphore;
        }

        submitInfo.pNext = &timelineInfo;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer[currentFrame];
        submitInfo.pWaitDstStageMask = waitStages;

        VkQueue graphicsQueue = GraphicsServices::GetContext()->GetVulkanDevice().GetGraphicsQueue();

		result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_inFlightFence[currentFrame]);

        if (result != VK_SUCCESS) {
            // Handle error — log it, abort, whatever
            printf("vkQueueSubmit failed with error %d\n", result);
        }

        uint64_t timeout = 100000000; // 100ms
        result = vkWaitForFences(device, 1, &m_inFlightFence[currentFrame], VK_TRUE, timeout);

        if (result == VK_TIMEOUT) {
            Logger::Log(Logger::ERROR, "Fence wait timed out after %llu ns", timeout);

            VkResult status = vkGetFenceStatus(device, m_inFlightFence[currentFrame]);
            if (status == VK_SUCCESS) {
                Logger::Log(Logger::WARN, "Fence appears to have signaled despite timeout.");
            } else if (status == VK_NOT_READY) {
                Logger::Log(Logger::ERROR, "Fence is still unsignaled (VK_NOT_READY). Possible GPU hang or bad submission.");
            } else {
                Logger::Log(Logger::ERROR, "vkGetFenceStatus returned error code: %d", status);
            }

        } else if (result == VK_SUCCESS) {
            //Logger::Log(Logger::INFO, "Fence signaled successfully within timeout.");
        } else {
            Logger::Log(Logger::ERROR, "vkWaitForFences failed with error: %d", result);
        }

        vkResetFences(device, 1, &m_inFlightFence[currentFrame]);
        

        if (m_bRenderToScreen)
        {
            uint32_t imageIndex = activeWindow->GetSwapChainImageIndex();
            VkQueue presentationQueue = GraphicsServices::GetContext()->GetVulkanDevice().GetPresentationQueue();
            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &m_renderFinishedSemaphore[currentFrame];
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &activeWindow->GetSwapChain();
            presentInfo.pImageIndices = &imageIndex;
    
            result = vkQueuePresentKHR(presentationQueue, &presentInfo);
    
            if (result != VK_SUCCESS) {
                // Handle error — log it, abort, whatever
                printf("present failed with error %d\n", result);
            }
        }
    }

    void LRenderTarget::blit(ARenderTarget& target)
    {
        //todo
    }

    void LRenderTarget::blit(unsigned int framebuffer)
    {
        //todo
    }

    void LRenderTarget::blitToScreen()
    {
        //todo
    }

    void LRenderTarget::SetColourAttachments(int count, bool multisampled)
    {
        for (int i = 0; i < count; ++i)
        {
            CreateColourAttachment((LColourAttachment)((int)LColourAttachment::LEMON_COLOR_ATTACHMENT0 + i), multisampled);
        }
    }

    void LRenderTarget::SetColourAttachments(const std::vector<LColourAttachment> attachments, bool multiSampled)
    {
        for (const LColourAttachment& attachment : attachments)
        {
            CreateColourAttachment(attachment, multiSampled);
        }
    }

    void LRenderTarget::AddMultiSampledDepthAttachment()
    {
        m_hasMultisampledColourAttachment = true;
        AddDepthAttachment(false);
    }

    uint32_t LRenderTarget::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        VkPhysicalDevice physicalDevice = GraphicsServices::GetContext()->GetVulkanDevice().GetPhysicalDevice();
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }


    void LRenderTarget::AddDepthAttachment(bool useRenderBufferStorage)
    {
        m_dirtyBuffer = true;
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        if (m_bHasDepthAttachment)
        {
            // delete depth attachment
            vkDestroyImage(device, m_depthAttachment.Image, nullptr);

            for (auto& imageview : m_depthAttachment.ImageViews)
            {
                vkDestroyImageView(device, imageview, nullptr);
            }
        }

        m_bHasDepthAttachment = true;

        uint32_t graphicsIndex = GraphicsServices::GetContext()->GetVulkanDevice().GetGraphicsQueueIndex();
        uint32_t queueFamilyIndices[] = { graphicsIndex };

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_depthDimensions.x;
        imageInfo.extent.height = m_depthDimensions.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = m_layerCount;
        imageInfo.format = VK_FORMAT_D32_SFLOAT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = m_hasMultisampledColourAttachment ? m_sampleCount : VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;
        imageInfo.pNext = nullptr;
        imageInfo.queueFamilyIndexCount = 1;
        imageInfo.pQueueFamilyIndices = queueFamilyIndices;

        VkImage depthImage;
        if (vkCreateImage(device, &imageInfo, nullptr, &depthImage) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create depth image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, depthImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkDeviceMemory depthMemory;
        if (vkAllocateMemory(device, &allocInfo, nullptr, &depthMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate memory for depth image!");
        }

        m_depthMemory = depthMemory;

        vkBindImageMemory(device, depthImage, depthMemory, 0);

        uint32 imageViewCount = m_bArrayTexture ? 1 : m_layerCount;
        m_depthAttachment.ImageViews.resize(imageViewCount);

        for (int i = 0; i < imageViewCount; ++i)
        {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = depthImage;
            //viewInfo.viewType = m_bArrayTexture ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.viewType = m_layerCount > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = VK_FORMAT_D32_SFLOAT;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = i;
            viewInfo.subresourceRange.layerCount = m_bArrayTexture ? m_layerCount : 1;
    
            VkImageView depthImageView;
            if (vkCreateImageView(device, &viewInfo, nullptr, &depthImageView) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create depth image view!");
            }
            
            m_depthAttachment.ImageViews[i] = depthImageView;
        }

        m_depthAttachment.Image = depthImage;
    }

    VulkanRenderTarget LRenderTarget::GetColourAttachment(LColourAttachment colourAttachment)
    {
        std::map<LColourAttachment, VulkanRenderTarget>::iterator iter = m_colourAttachments.find(colourAttachment);

        if (iter != m_colourAttachments.end())
        {
            return iter->second;
        }

        return VulkanRenderTarget();
    }

    uint LRenderTarget::CreateColourAttachment(LColourAttachment colourAttachment, bool multisampled, int internalFormat)
    {
        m_dirtyBuffer = true;
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

        m_hasMultisampledColourAttachment = multisampled;
        std::map<LColourAttachment, VulkanRenderTarget>::iterator iter = m_colourAttachments.find(colourAttachment);

        if (iter != m_colourAttachments.end())
        {
            for (auto& imageView : iter->second.ImageViews)
            {
                vkDestroyImageView(device, imageView, nullptr);
            }

            vkDestroyImage(device, iter->second.Image, nullptr);
        }

        m_colourAttachments[colourAttachment] = VulkanRenderTarget();
        
        uint32_t graphicsIndex = GraphicsServices::GetContext()->GetVulkanDevice().GetGraphicsQueueIndex();
        uint32_t queueFamilyIndices[] = { graphicsIndex };

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_colourDimensions.x;
        imageInfo.extent.height = m_colourDimensions.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = m_layerCount;
        imageInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = multisampled ? m_sampleCount : VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0; 
        imageInfo.pNext = nullptr;
        imageInfo.queueFamilyIndexCount = 1;
        imageInfo.pQueueFamilyIndices = queueFamilyIndices;

        if (vkCreateImage(device, &imageInfo, nullptr, &m_colourAttachments.at(colourAttachment).Image) != VK_SUCCESS) {
            Logger::Log(Logger::ERROR,"Failed to create image!");
            throw std::runtime_error("Failed to create image!");
            return -1;
        }
        
        VulkanRenderTarget target = m_colourAttachments.at(colourAttachment);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, target.Image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &target.Memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate memory for depth image!");
        }

        vkBindImageMemory(device, target.Image, target.Memory, 0);

        // With an array texture we have a single image view otherwise we have 1 image view per layer.
        uint32 imageViewCount = m_bArrayTexture ? 1 : m_layerCount;
        m_colourAttachments.at(colourAttachment).ImageViews.resize(imageViewCount);

        for (int i = 0; i < imageViewCount; ++i)
        {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT; 
            viewInfo.viewType = m_bArrayTexture ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D; 
            viewInfo.image = m_colourAttachments.at(colourAttachment).Image;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = i;
            viewInfo.subresourceRange.layerCount = m_bArrayTexture ? m_layerCount  : 1;
    
            if (vkCreateImageView(device, &viewInfo, nullptr, &m_colourAttachments.at(colourAttachment).ImageViews[i]) != VK_SUCCESS) {
                Logger::Log(Logger::ERROR,"Failed to create image view!");
                throw std::runtime_error("Failed to create image view!");
                return -1;
            }
        }

        if (m_linearSampler == VK_NULL_HANDLE)
        {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_FALSE;
            samplerInfo.maxAnisotropy = 1.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    
            if (vkCreateSampler(device, &samplerInfo, nullptr, &m_linearSampler)!= VK_SUCCESS) {
                Logger::Log(Logger::ERROR,"Failed to create image sampler!");
                throw std::runtime_error("Failed to create image sampler!");
                return -1;
            }
        }

        return 0;
    }

    void LRenderTarget::SetRenderBlock(ARenderBlock* block )
    { 
        if (block == m_renderBlock)
        {
            return;
        }

        m_renderBlock = block;
        // :o descriptors will needs to made dirty with the current configuration could lead to descriptors being updated more than necessary, refactor candidate.
        m_descriptorsDirty = true;
    }

    void LRenderTarget::UpdateDescriptorSets(VkDescriptorSet dstSet)
    {
        if (m_renderBlock == nullptr)
        {
            Logger::Log(Logger::WARN, "Unable to update descriptor set for null render block");
            return;
        }

        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        CitrusCore::ResourcePtr<Material> material = m_renderBlock->GetMaterial();
        LRenderBlock* renderBlock = static_cast<LRenderBlock*>(m_renderBlock);

        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorImageInfo> imageDescriptors;
        imageDescriptors.reserve(m_colourAttachments.size());
        uint32_t currentFrame = GraphicsServices::GetWindowManager()->GetActiveWindow()->GetCurrentFrame();
        LVKBuffer buffer = renderBlock->GetLVKBuffer(currentFrame);

        int attachmentCount = 0;
        int minIndex = INT_MAX; 

        for (const auto& texture : material->GetResource()->GetTextures())
        {
            if (texture.second->GetBindLocation() < minIndex)
            {
                minIndex = texture.second->GetBindLocation();
            }
        }

		for (const auto& texture : material->GetResource()->GetTextures())
		{
			Texture* tex = static_cast<Texture*>(texture.second->GetTexture()->GetResource());
			uint32_t texturebindLocation = texture.second->GetBindLocation();
            uint32_t attachmentIndex = texturebindLocation - minIndex;
            const auto& attachment = m_colourAttachments.at(static_cast<LColourAttachment>(((uint)LColourAttachment::LEMON_COLOR_ATTACHMENT0 + attachmentIndex)));

            // Create image descriptor 
            VkDescriptorImageInfo imageDescriptor = {};
            imageDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageDescriptor.imageView = attachment.ImageViews[0]; /// This should be the index of the layer we want to bind when not using array texture?
            imageDescriptors.push_back(imageDescriptor);

            VkWriteDescriptorSet writeImage = {};
            writeImage.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeImage.dstSet = dstSet;
            writeImage.dstBinding = texturebindLocation;
            writeImage.dstArrayElement = 0;
            writeImage.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            writeImage.descriptorCount = 1;
            writeImage.pImageInfo = &imageDescriptors.back();
            writes.push_back(writeImage);
        }

        vkUpdateDescriptorSets(device, writes.size(), writes.data(), 0, nullptr);
    }

    void LRenderTarget::Clear(uint clearFlags)
    {
    }

    ARenderTarget* LRenderTarget::GetScreenTarget(LWindow* window)
    {
        if (window == nullptr)
        {
            throw std::runtime_error("You cannot get the rendertarget for a null window.");
        }

        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        auto iter = m_defaultTargets.find(window->GetUID());

        if (iter == m_defaultTargets.end())
        {
            // Get swapchain images for window 
            const std::vector<VkImage>& images = window->GetSwapChainImages();

            if (images.empty())
            {
                throw std::runtime_error("Swapchain not created!?, unable to create render target for swapchain.");
            }

            for (VkImage image : images)
            {
                // Create a new render target.
                std::shared_ptr<LRenderTarget> renderTarget = std::make_shared<LRenderTarget>(glm::ivec2(window->GetWidth(), window->GetHeight()));
                renderTarget->InitAsDefault();
                renderTarget->Init();
                m_defaultTargets[window->GetUID()].push_back(renderTarget);

                // Add a single colour attachment to new render target -> Set this to the swap chain image/ image view for the swapchain image.
                renderTarget->m_colourAttachments[LColourAttachment::LEMON_COLOR_ATTACHMENT0] = VulkanRenderTarget();
                renderTarget->m_colourAttachments[LColourAttachment::LEMON_COLOR_ATTACHMENT0].Image = image;

                // Create the image views from the swapchain images.
                VkImageViewCreateInfo viewInfo = {};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = image;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; 
                viewInfo.format = window->GetSwapChainImageFormat();
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;

                renderTarget->m_colourAttachments[LColourAttachment::LEMON_COLOR_ATTACHMENT0].ImageViews.resize(1);

                if (vkCreateImageView(device, &viewInfo, nullptr, &renderTarget->m_colourAttachments[LColourAttachment::LEMON_COLOR_ATTACHMENT0].ImageViews[0]) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create image views for swapchain");
                }         
            }
        }

        // Return target for current swap chain frame. 
        return m_defaultTargets[window->GetUID()][window->GetSwapChainImageIndex()].get();
    }
}

#endif 
