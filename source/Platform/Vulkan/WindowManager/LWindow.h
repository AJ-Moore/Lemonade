#pragma once

#include <LCommon.h>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
#ifdef RENDERER_VULKAN

#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Platform/Core/WindowManager/LSDLWindow.h>

namespace Lemonade
{
	class LEMONADE_API LWindow : public LSDLWindow {
		friend class LWindowManager; 
	public:
		virtual ~LWindow(){}
		VkSurfaceKHR GetVkSurface() { return m_vkSurface; }
		VkImage GetSwapChainImage() const { return m_swapChainImages[m_activeSwapChainImageIndex]; }
		uint32_t GetSwapChainImageIndex() const { return m_activeSwapChainImageIndex; }

		const std::vector<VkImage>& GetSwapChainImages() const { return m_swapChainImages; }
		uint32_t GetCurrentFrame() { return m_currentFrame; }
		VkFormat GetSwapChainImageFormat() const { return m_imageFormat; }
		const VkSwapchainKHR& GetSwapChain() const { return m_swapChain; }
		void WaitForFence();
		VkFence GetFence() { return m_fence[m_currentFrame]; }
		VkSemaphore GetSemaphore(){ return m_imageAvailableSemaphore[m_currentFrame]; }
		VkSemaphore GetTimelineSemaphore() { return m_timelineSemaphore[m_currentFrame]; }
		uint64_t GetFrameSemaphoreTimelineValueAndIncrement();
		uint64_t GetPassIndexAndIncrement();
	protected:
		virtual void Unload() override;
		virtual bool Init() override;
		virtual void Render() override;
	private:
		void CreateSwapChain();

		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkAllocationCallbacks allocationCallbacks;		
		uint32_t m_imageCount = 0;
		uint32_t m_activeSwapChainImageIndex = 0;
		std::vector<VkImage> m_swapChainImages;
		VkFence m_fence[4];
		VkSemaphore m_imageAvailableSemaphore[4];
		VkSemaphore m_timelineSemaphore[4];
		VkFormat m_imageFormat;
		uint32_t m_currentFrame = 0;
		uint32_t m_passIndex = 0;
		uint64_t m_currentSemaphoreValue = 0;
	};
}

#endif