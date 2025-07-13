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

	void Texture::LoadNativeTextureFromSurface(SDL_Surface* surface)
	{
		LoadNativeTextureFromPixels(static_cast<Colour*>(surface->pixels), surface->w, surface->h);
	}

	void Texture::LoadNativeTextureFromPixels(const Colour* pixels, uint32_t width, uint32_t height)
	{
		VkFormat format = (VkFormat) TextureData::GetNativeTextureFormat(GetTextureFormat());
		VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

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

		void* mappedData;
		vkMapMemory(device, m_tempDeviceMemoryForCopy, 0, bufferSize, 0, &mappedData);
		memcpy(mappedData, pixels, bufferSize);
		m_pendingCopy = true;
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