#include "Platform/Core/Time/Time.h"
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Util/Logger.h>
#include <LCommon.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

#ifdef RENDERER_VULKAN

#include <memory>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>

namespace Lemonade
{
    using CitrusCore::Logger;

    std::unordered_map<CitrusCore::UID, std::vector<std::shared_ptr<LRenderTarget>>> LRenderTarget::m_defaultTargets;

    // TODO remove this hack, thanks
    LRenderTarget& LRenderTarget::GetDefault()
    {
        static LRenderTarget target; 
        return target;
    }

    LRenderTarget::LRenderTarget()
    {
    }

    LRenderTarget::LRenderTarget(glm::ivec2 dimensions)
    {
        m_dimensions = dimensions;
    }

    LRenderTarget::~LRenderTarget()
    {
        if (m_frameBuffer)
        {
            VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
            vkDestroyFramebuffer(device, m_frameBuffer, nullptr);
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


        return true;
    }

    void LRenderTarget::bindColourAttachments()
    {
    }

    void LRenderTarget::bindColourAttachment(LColourAttachment colourAttachment, uint activeTarget)
    {
    }

    void LRenderTarget::bindDepthAttachment(uint activeTarget)
    {
    }

    void LRenderTarget::GenerateBuffers()
    {
        // Generate Vulkan Framebuffer

        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

        // Destroy framebuffer if it exists
        if (m_frameBuffer)
        {
			vkDestroyFramebuffer(device, m_frameBuffer, nullptr);
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
            colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
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
			attachmentRef.attachment = attachmentCount;
			attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentRefs.push_back(attachmentRef);
		}

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = m_colourAttachments.size();;
        subpass.pColorAttachments = attachmentRefs.data();

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = m_colourAttachments.size();
        renderPassInfo.pAttachments = attachmentDescription.data();
        renderPassInfo.subpassCount = 1; 
        renderPassInfo.pSubpasses = &subpass; 

        vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass);

        VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = m_colourAttachments.size();
		framebufferInfo.width = m_dimensions.x;
		framebufferInfo.height = m_dimensions.y;
		framebufferInfo.layers = 1;

		int i = 0;
        std::vector<VkImageView> images;

        for (const auto& target : m_colourAttachments)
        {
            images.push_back(target.second.ImageView);
		}

        framebufferInfo.pAttachments = images.data();

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffer) != VK_SUCCESS)
        {
			Logger::Log(Logger::ERROR,"Failed to create framebuffer!");
			throw std::runtime_error("Failed to create framebuffer!");
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

    void LRenderTarget::BeginRenderPass()
    {
        // If buffer dirty or not generated, generate it. Step was to reduce uneccessary buffer re-generation when adding colour targets & such.
        if (m_dirtyBuffer)
        {
            GenerateBuffers();
        }

        GraphicsServices::GetRenderer()->SetActiveRenderTarget(this);
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

        VkClearValue clearValues[2];
        clearValues[0].color = { { m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a } };

        if (m_bHasDepthAttachment)
        {
            clearValues[1].depthStencil = {1.0f, 0};   
        }

        LWindow* activeWindow = GraphicsServices::GetWindowManager()->GetActiveWindow();
        uint32_t currentFrame = activeWindow->GetCurrentFrame();

        m_activeBuffer = m_commandBuffer[currentFrame];

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_frameBuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent.width = m_dimensions.x;
        renderPassInfo.renderArea.extent.height = m_dimensions.y;
        renderPassInfo.pClearValues = clearValues;
        renderPassInfo.clearValueCount = (m_bHasDepthAttachment) ? 2 : 1;

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Set command buffer usage flags
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkResetCommandBuffer(m_commandBuffer[currentFrame], 0); 
        vkBeginCommandBuffer(m_commandBuffer[currentFrame], &beginInfo);
        vkCmdBeginRenderPass(m_commandBuffer[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // TODO replace with viewport apply method
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)800;
        viewport.height = (float)600;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        vkCmdSetViewport(m_commandBuffer[currentFrame], 0, 1, &viewport);
        
        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = {800, 600};
        
        vkCmdSetScissor(m_commandBuffer[currentFrame], 0, 1, &scissor);
    }

    void LRenderTarget::EndRenderPass()
    {
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        VkResult result;

        LWindow* activeWindow = GraphicsServices::GetWindowManager()->GetActiveWindow();
        uint32_t currentFrame = activeWindow->GetCurrentFrame();
        
        vkCmdEndRenderPass(m_commandBuffer[currentFrame]);
        VkResult endResult = vkEndCommandBuffer(m_commandBuffer[currentFrame]);
        
        vkEndCommandBuffer(m_commandBuffer[currentFrame]);
        if (endResult != VK_SUCCESS) {
            Logger::Log(Logger::ERROR, "Failed to end command buffer: %d", endResult);
            return; // don't submit if ending failed
        }
        
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };        
        
        VkSemaphore semaphore = activeWindow->GetSemaphore();
        //VkFence inFlightFrame = activeWindow->GetFence();

        uint64_t value = activeWindow->GetFrameSemaphoreTimelineValueAndIncrement();
        uint64_t nextValue = value + 1;

        VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore[currentFrame], semaphore };
        uint64_t signalValues[] = {0,value + 1 }; 

        VkTimelineSemaphoreSubmitInfo timelineInfo{};
        timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        timelineInfo.signalSemaphoreValueCount = 2;
        timelineInfo.pSignalSemaphoreValues = signalValues;
        timelineInfo.pWaitSemaphoreValues = &value;
        timelineInfo.waitSemaphoreValueCount = 1;

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        if (m_bRenderToScreen)
        {
            submitInfo.signalSemaphoreCount = 2;
            submitInfo.pSignalSemaphores = signalSemaphores;
        }
        else {
            timelineInfo.signalSemaphoreValueCount = 1;
            timelineInfo.pSignalSemaphoreValues = &signalValues[1];
            submitInfo.signalSemaphoreCount = 1; 
            submitInfo.pSignalSemaphores = &semaphore;
        }

        submitInfo.pNext = &timelineInfo;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer[currentFrame];
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &semaphore;
        submitInfo.pWaitDstStageMask = waitStages;

        VkQueue graphicsQueue = GraphicsServices::GetContext()->GetVulkanDevice().GetGraphicsQueue();

		result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_inFlightFence[currentFrame]);

        if (result != VK_SUCCESS) {
            // Handle error — log it, abort, whatever
            printf("vkQueueSubmit failed with error %d\n", result);
        }

		//vkQueueWaitIdle(graphicsQueue);

        // Wait until frame finished rendering.
        //activeWindow->WaitForFence();

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
            Logger::Log(Logger::INFO, "Fence signaled successfully within timeout.");
        } else {
            Logger::Log(Logger::ERROR, "vkWaitForFences failed with error: %d", result);
        }

        //vkWaitForFences(device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
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

        //vkQueueSubmit(graphicsQueue, 0, nullptr, m_inFlightFence);
//
        //if (result != VK_SUCCESS) {
        //    // Handle error — log it, abort, whatever
        //    printf("vkQueueSubmit failed with error %d\n", result);
        //}
//
        //vkWaitForFences(device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
        //vkResetFences(device, 1, &m_inFlightFence);


        //vkQueueWaitIdle(presentationQueue);
    }

    void LRenderTarget::blit(ARenderTarget& target)
    {
        //LRenderTarget* rtarget = (LRenderTarget*)&target;
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rtarget->m_frameBuffer);
        //glBlitFramebuffer(0, 0, m_dimensions.x, m_dimensions.y, 0, 0, m_dimensions.x, m_dimensions.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void LRenderTarget::blit(unsigned int framebuffer)
    {
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
        //glBlitFramebuffer(0, 0, m_dimensions.x, m_dimensions.y, 0, 0, m_dimensions.x, m_dimensions.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void LRenderTarget::blitToScreen()
    {
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        //glBlitFramebuffer(0, 0, m_dimensions.x, m_dimensions.y, 0, 0, m_dimensions.x, m_dimensions.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void LRenderTarget::setDimensions(glm::ivec2 dimensions)
    {
        if (dimensions == m_dimensions)
        {
            return;
        }

        m_dimensions = dimensions;
        m_dirtyBuffer = true;
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

    void LRenderTarget::addMultiSampledDepthAttachment()
    {
        m_hasMultisampledColourAttachment = true;
        AddDepthAttachment(false, 1);
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


    void LRenderTarget::AddDepthAttachment(bool useRenderBufferStorage, int layers)
    {
        m_dirtyBuffer = true;
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        if (m_bHasDepthAttachment)
        {
            // delete depth attachment
            vkDestroyImage(device, m_depthAttachment.Image, nullptr);
            vkDestroyImageView(device, m_depthAttachment.ImageView, nullptr);

        }

        m_bHasDepthAttachment = true;

        uint32_t graphicsIndex = GraphicsServices::GetContext()->GetVulkanDevice().GetGraphicsQueueIndex();
        uint32_t queueFamilyIndices[] = { graphicsIndex };

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_dimensions.x;
        imageInfo.extent.height = m_dimensions.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_D32_SFLOAT; // Depth format
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

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_D32_SFLOAT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView depthImageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &depthImageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create depth image view!");
        }

        m_depthAttachment.Image = depthImage;
        m_depthAttachment.ImageView = depthImageView;
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
            vkDestroyImageView(device, iter->second.ImageView, nullptr);
            vkDestroyImage(device, iter->second.Image, nullptr);
        }

        m_colourAttachments[colourAttachment] = VulkanRenderTarget();
        
        uint32_t graphicsIndex = GraphicsServices::GetContext()->GetVulkanDevice().GetGraphicsQueueIndex();
        uint32_t queueFamilyIndices[] = { graphicsIndex };

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_dimensions.x;
        imageInfo.extent.height = m_dimensions.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM; 
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; 
        viewInfo.image = m_colourAttachments.at(colourAttachment).Image;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &m_colourAttachments.at(colourAttachment).ImageView) != VK_SUCCESS) {
            Logger::Log(Logger::ERROR,"Failed to create image view!");
            throw std::runtime_error("Failed to create image view!");
            return -1;
        }

        return 0;
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

                if (vkCreateImageView(device, &viewInfo, nullptr, &renderTarget->m_colourAttachments[LColourAttachment::LEMON_COLOR_ATTACHMENT0].ImageView) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create image views for swapchain");
                }         
            }
        }

        // Return target for current swap chain frame. 
        return m_defaultTargets[window->GetUID()][window->GetCurrentFrame()].get();
    }
}

#endif 
