


#ifdef RENDERER_VULKAN
#include <Renderer/ARenderTarget.h>
#include <Core/UServiceLocator.h>
#include <Renderer/Platform/Vulkan/URenderTarget_vulkan.h>

namespace UpsilonEngine
{
    UniquePtr<URenderTarget> URenderTarget::m_defaultTarget = nullptr;

    URenderTarget::URenderTarget()
    {
    }

    URenderTarget::URenderTarget(glm::ivec2 dimensions)
    {
        m_dimensions = dimensions;
    }

    URenderTarget::~URenderTarget()
    {
        if (m_frameBuffer)
        {
            VkDevice device = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getVkDevice();
            vkDestroyFramebuffer(device, m_frameBuffer, nullptr);
        }
    }

    bool URenderTarget::init()
    {
        if (m_bDoneInit)
        {
            return true;
        }

        m_bDoneInit = true;
        return true;
    }

    void URenderTarget::bindColourAttachments()
    {
    }

    void URenderTarget::bindColourAttachment(UColourAttachment colourAttachment, uint activeTarget)
    {
    }

    void URenderTarget::bindDepthAttachment(uint activeTarget)
    {
    }

    void URenderTarget::generateBuffers()
    {
        // Generate Vulkan Framebuffer

        VkDevice device = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getVkDevice();

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
			LogError("Failed to create framebuffer!");
			throw std::runtime_error("Failed to create framebuffer!");
		}

        // Create command buffer
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getGraphicsCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        if (vkAllocateCommandBuffers(device, &allocInfo, &m_commandBuffer) != VK_SUCCESS)
        {
			LogError("Failed to allocate command buffer!");
			throw std::runtime_error("Failed to allocate command buffers!");
		}

		m_dirtyBuffer = false;
    }

    void URenderTarget::beginRenderPass()
    {
        // If buffer dirty or not generated, generate it. Step was to reduce uneccessary buffer re-generation when adding colour targets & such.
        if (m_dirtyBuffer)
        {
            generateBuffers();
        }

        VkDevice device = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getVkDevice();

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_frameBuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent.width = m_dimensions.x;
        renderPassInfo.renderArea.extent.height = m_dimensions.y;


        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Set command buffer usage flags
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
    }

    void URenderTarget::endRenderPass()
    {
        vkEndCommandBuffer(m_commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer;

		vkQueueSubmit(UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getGraphicsQueue());
    }

    void URenderTarget::blit(ARenderTarget& target)
    {
        //URenderTarget* rtarget = (URenderTarget*)&target;
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rtarget->m_frameBuffer);
        //glBlitFramebuffer(0, 0, m_dimensions.x, m_dimensions.y, 0, 0, m_dimensions.x, m_dimensions.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void URenderTarget::blit(unsigned int framebuffer)
    {
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
        //glBlitFramebuffer(0, 0, m_dimensions.x, m_dimensions.y, 0, 0, m_dimensions.x, m_dimensions.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void URenderTarget::blitToScreen()
    {
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        //glBlitFramebuffer(0, 0, m_dimensions.x, m_dimensions.y, 0, 0, m_dimensions.x, m_dimensions.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void URenderTarget::setDimensions(glm::ivec2 dimensions)
    {
        if (dimensions == m_dimensions)
        {
            return;
        }

        m_dimensions = dimensions;
        m_dirtyBuffer = true;
    }

    void URenderTarget::setColourAttachments(const std::vector<UColourAttachment> attachments, bool multiSampled)
    {
        for (const UColourAttachment& attachment : attachments)
        {
            createColourAttachment(attachment, multiSampled);
        }
    }

    void URenderTarget::addMultiSampledDepthAttachment()
    {
        m_hasMultisampledColourAttachment = true;
        addDepthAttachment(false, 1);
    }

    uint32_t URenderTarget::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        VkPhysicalDevice physicalDevice = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getPhysicalDevice();
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
    }


    void URenderTarget::addDepthAttachment(bool useRenderBufferStorage, int layers)
    {
        m_dirtyBuffer = true;
        VkDevice device = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getVkDevice();
        if (m_bHasDepthAttachment)
        {
            // delete depth attachment
            vkDestroyImage(device, m_depthAttachment.Image, nullptr);
            vkDestroyImageView(device, m_depthAttachment.ImageView, nullptr);

        }

        m_bHasDepthAttachment = true;

        uint32_t graphicsIndex = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getGraphicsQueueIndex();
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

    VulkanRenderTarget URenderTarget::getColourAttachment(UColourAttachment colourAttachment)
    {
        std::map<UColourAttachment, VulkanRenderTarget>::iterator iter = m_colourAttachments.find(colourAttachment);

        if (iter != m_colourAttachments.end())
        {
            return iter->second;
        }

        return VulkanRenderTarget();
    }

    uint URenderTarget::createColourAttachment(UColourAttachment colourAttachment, bool multisampled, int internalFormat)
    {
        m_dirtyBuffer = true;
        VkDevice device = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getVkDevice();

        m_hasMultisampledColourAttachment = multisampled;
        std::map<UColourAttachment, VulkanRenderTarget>::iterator iter = m_colourAttachments.find(colourAttachment);

        if (iter != m_colourAttachments.end())
        {
            vkDestroyImageView(UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getVkDevice(), iter->second.ImageView, nullptr);
            vkDestroyImage(UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getVkDevice(), iter->second.Image, nullptr);
        }

        m_colourAttachments[colourAttachment] = VulkanRenderTarget();
        
        uint32_t graphicsIndex = UServiceLocator::getInstance()->getWindowManager()->getVulkanDevice()->getGraphicsQueueIndex();
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
            LogError("Failed to create image!");
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
            LogError("Failed to create image view!");
            throw std::runtime_error("Failed to create image view!");
            return -1;
        }

        return 0;
    }

    void URenderTarget::clear(uint clearFlags)
    {
        beginRenderPass();
        GLuint flags = 0;
        flags |= (clearFlags & (uint)UBufferBit::COLOUR) ? GL_COLOR_BUFFER_BIT : 0;
        flags |= (clearFlags & (uint)UBufferBit::DEPTH) ? GL_DEPTH_BUFFER_BIT : 0;
        glClearColor(m_clearColour.x, m_clearColour.y, m_clearColour.z, m_clearColour.w);
        glClear(flags);
        LogGLErrors();
    }

    ARenderTarget* URenderTarget::GetScreenTarget()
    {
        if (m_defaultTarget == nullptr)
        {
            m_defaultTarget = std::make_unique<URenderTarget>();
            m_defaultTarget->initAsDefault();
        }

        createColourAttachment(UColourAttachment::UP_COLOR_ATTACHMENT0, false);

        return m_defaultTarget.get();
    }
}

#endif 
