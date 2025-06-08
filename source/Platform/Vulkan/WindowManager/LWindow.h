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
	protected:
		virtual void Unload() override;
		virtual bool Init() override;
		virtual void Render() override;
	private:
		void CreateSwapChain();
		void CreateVkPipeline();

		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkAllocationCallbacks allocationCallbacks;		
		uint32_t m_imageCount = 0;
		uint32_t m_activeSwapChainImageIndex = 0;
		std::vector<VkImage> m_swapChainImages;
		VkFence m_fence = VK_NULL_HANDLE;
	};
}

#endif