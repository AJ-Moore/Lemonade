#pragma once

#include "Platform/Core/Renderer/Materials/TextureUtils.h"
#include <LCommon.h>

#ifdef RENDERER_VULKAN
#include <unordered_set>
#include <vulkan/vulkan_core.h>
#include <Platform/Core/Renderer/Materials/ATexture.h>


namespace Lemonade
{
	class LEMONADE_API Texture : public ATexture
	{
	public:
		virtual void Bind() override{}
		virtual void Bind(uint textureUnit) override{}
		virtual void LoadNativeTextureFromSurface(SDL_Surface* surface) override;
		virtual void LoadNativeTextureFromPixels(const Colour* data, uint32_t width, uint32_t height, TextureFormat textureformat = TextureFormat::LEMONADE_FORMAT_RGBA8888) override;

		void UpdateVKImage(VkCommandBuffer commandBuffer);
		void UpdateTextureWith(VkImage image, VkSampler imageSampler, VkImageView imageView);

		VkImageView GetImageView() const noexcept { return m_imageView; }
		VkSampler GetImageSampler() const noexcept { return m_imageSampler; }

		static void UploadTextures(VkCommandBuffer cmdBuffer);
	protected:
		virtual void UnloadResource() override;
	private:
		// Keep track of textures pending data upload to gpu memory
		static std::unordered_set<Texture*> m_texturesForUpload;

		VkBuffer m_stagingBuffer;
		VkImage m_image;
		VkImageView m_imageView;
		VkSampler m_imageSampler;
		VkDeviceMemory m_imageMemory;
		VkDeviceMemory m_tempDeviceMemoryForCopy;
		bool m_pendingCopy = false;
	};
}

#endif