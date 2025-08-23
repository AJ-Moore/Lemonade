#include "Platform/Core/Renderer/Materials/ATexture.h"
#include "Platform/Core/Renderer/Materials/TextureUtils.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include <LCommon.h>
#include <cmath>
#include <cstdint>
#include <vulkan/vulkan_core.h>

#ifdef RENDERER_VULKAN
#include <Platform/Vulkan/Materials/Texture.h>
#include <Platform/Core/Renderer/Materials/TextureType.h>

namespace Lemonade {

	std::unordered_set<Texture*> Texture::m_texturesForUpload;

	// Native Texture Format Lookup 
	const std::unordered_map<TextureFormat, int> TextureData::m_nativeTextureFormatLookup = {
	{ TextureFormat::LEMONADE_RGBA8888, VK_FORMAT_R8G8B8A8_UNORM },
	{ TextureFormat::LEMONADE_BGRA8888, VK_FORMAT_B8G8R8A8_UNORM },
	{ TextureFormat::LEMONADE_RGB888, VK_FORMAT_R8G8B8_UNORM },
	{ TextureFormat::LEMONADE_BGR888, VK_FORMAT_B8G8R8_UNORM },
	{ TextureFormat::LEMONADE_UNKNOWN, VK_FORMAT_UNDEFINED },
	};

	int TextureData::GetNativeTextureFormat(const TextureFormat& format)
	{
		std::unordered_map<TextureFormat, int>::const_iterator iter = m_nativeTextureFormatLookup.find(format);
		
		if (iter != m_nativeTextureFormatLookup.end())
		{
			return iter->second;
		}

		return -1;
	}

	void Texture::UploadTextures(VkCommandBuffer cmdBuffer)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		std::vector<VkImageMemoryBarrier> barriers;
		for (auto& texture : m_texturesForUpload)
		{
			barrier.image = texture->m_image;
			barriers.push_back(barrier);
		}

		vkCmdPipelineBarrier(
			cmdBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			static_cast<uint32_t>(barriers.size()), barriers.data()
		);

		for (auto& texture : m_texturesForUpload)
		{
			texture->UpdateVKImage(cmdBuffer);
		}

		for (auto& bar : barriers)
		{
			bar.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			bar.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			bar.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			bar.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}
		
		vkCmdPipelineBarrier(
			cmdBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			static_cast<uint32_t>(barriers.size()), barriers.data()
		);

		m_texturesForUpload.clear();
	}

	void Texture::LoadNativeTextureFromSurface(SDL_Surface* surface)
	{
		LoadNativeTextureFromPixels(static_cast<Colour*>(surface->pixels), surface->w, surface->h);
	}

	void Texture::LoadNativeTextureFromPixels(const Colour* pixels, uint32_t width, uint32_t height)
	{
		m_width = width; 
		m_height = height;
		VkFormat format = (VkFormat) TextureData::GetNativeTextureFormat(GetTextureFormat());
		VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
		VkPhysicalDevice physicalDevice = GraphicsServices::GetContext()->GetVulkanDevice().GetPhysicalDevice();

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		vkCreateImage(device, &imageInfo, nullptr, &m_image);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkMemoryRequirements imageMemRequirements;
        vkGetImageMemoryRequirements(device, m_image, &imageMemRequirements);

        VkMemoryAllocateInfo imgallocInfo = {};
        imgallocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        imgallocInfo.allocationSize = imageMemRequirements.size;

		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		uint32_t properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		uint32_t memoryTypeIndex = 0;

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((imageMemRequirements.memoryTypeBits & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				memoryTypeIndex = i;
			}
		}

        imgallocInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(device, &imgallocInfo, nullptr, &m_imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate memory for depth image!");
        }

        vkBindImageMemory(device, m_image, m_imageMemory, 0);

		vkCreateImageView(device, &viewInfo, nullptr, &m_imageView);

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

		vkCreateSampler(device, &samplerInfo, nullptr, &m_imageSampler);

		uint32_t bufferSize =  sizeof(Colour) * width * height;
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		vkCreateBuffer(device, &bufferInfo, nullptr, &m_stagingBuffer);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_stagingBuffer,&memRequirements);

		properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		memoryTypeIndex = 0;

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memRequirements.memoryTypeBits & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				memoryTypeIndex = i;
			}
		}

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		assert(allocInfo.memoryTypeIndex < memProps.memoryTypeCount);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_tempDeviceMemoryForCopy) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate texture memory!");
		}

		vkBindBufferMemory(device, m_stagingBuffer, m_tempDeviceMemoryForCopy, 0);

		void* mappedData;
		vkMapMemory(device, m_tempDeviceMemoryForCopy, 0, bufferSize, 0, &mappedData);
		memcpy(mappedData, pixels, bufferSize);
		m_pendingCopy = true;
		m_texturesForUpload.emplace(this);
	}

	void Texture::UnloadResource() 
	{
		ATexture::UnloadResource();

		VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
		vkUnmapMemory(device, m_imageMemory);
	}

	void Texture::UpdateTextureWith(VkImage image, VkSampler imageSampler, VkImageView imageView)
	{
		m_image = image; 
		m_imageView = imageView; 
		m_imageSampler = imageSampler;
		VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
		vkUnmapMemory(device, m_tempDeviceMemoryForCopy);
		m_pendingCopy = false;
	}

	void Texture::UpdateVKImage(VkCommandBuffer commandBuffer)
	{
		if (!m_pendingCopy)
		{
			return;
		}

		VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;    // Tightly packed
		region.bufferImageHeight = 0;
		
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = { m_width, m_height, 1 };

		vkCmdCopyBufferToImage(
			commandBuffer,
			m_stagingBuffer,
			m_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		vkUnmapMemory(device, m_tempDeviceMemoryForCopy);

		m_pendingCopy = false;
	}
}

#endif 