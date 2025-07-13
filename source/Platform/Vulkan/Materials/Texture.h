#pragma once

#include <LCommon.h>
#include <vulkan/vulkan_core.h>
#ifdef RENDERER_VULKAN
#include <Platform/Core/Renderer/Materials/ATexture.h>


namespace Lemonade
{
	class LEMONADE_API Texture : public ATexture
	{
	public:
		virtual void Bind(){}
		virtual void Bind(uint textureUnit){}
		virtual void LoadNativeTextureFromSurface(SDL_Surface* surface);
		virtual void LoadNativeTextureFromPixels(const Colour* data, uint32_t width, uint32_t height);

		void UpdateVKImage(VkCommandBuffer commandBuffer);
	private:
		VkBuffer m_stagingBuffer;
		VkImage m_image;
		VkImageView m_imageView;
		VkSampler m_imageSampler;
		VkDeviceMemory m_tempDeviceMemoryForCopy;
		bool m_pendingCopy = false;
	};
}

#endif