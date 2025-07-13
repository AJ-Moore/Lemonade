#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/Core/LGraphicsContext.h>
#include "Platform/Core/WindowManager/LSDLWindow.h"
#include "Platform/Vulkan/Renderer/LRenderTarget.h"
#include "SDL3/SDL_vulkan.h"
#include "Util/Logger.h"
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <SDL3/SDL_vulkan.h>
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Lemonade 
{
    // Custom allocation functions
    void* CustomAllocate(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
		return malloc(size);  // Example: just use malloc for allocations
    }

    void* CustomReallocate(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
		return realloc(pOriginal, size);  // Example: just use realloc for reallocations
    }

    void CustomFree(void* pUserData, void* pMemory) {
		free(pMemory);  // Example: just use free for deallocations
    }

    void LWindow::Unload()
    {
        std::shared_ptr<LGraphicsContext> vulkanContext = GraphicsServices::GetContext();
        vkDestroySurfaceKHR(vulkanContext->GetVkInstance(), m_vkSurface, nullptr);
		vkDestroySwapchainKHR(vulkanContext->GetVulkanDevice().GetVkDevice(), m_swapChain, nullptr);

		for (int i = 0; i < LRenderTarget::MAX_FRAMES_IN_FLIGHT; ++i) {
			vkDestroyFence(vulkanContext->GetVulkanDevice().GetVkDevice(), m_fence[i], nullptr);
			vkDestroySemaphore(vulkanContext->GetVulkanDevice().GetVkDevice(), m_imageAvailableSemaphore[i], nullptr);
		}
    }

	void LWindow::Render()
	{
		VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

		// Check if timeline semaphore is waiting to be signaled, not equal to 0 almost certainly triggered 
		//if (m_currentSemaphoreValue != 0)
		//{
		//	uint64_t currentValue = 0;
		//	vkGetSemaphoreCounterValue(device, m_timelineSemaphore[m_currentFrame], &currentValue);
		//	uint64_t waitvalue = m_currentSemaphoreValue;
//
		//	//if (currentValue < waitvalue)
		//	{
		//		// Still waiting — semaphore hasn't been signaled to targetValue yet
//
		//		VkSemaphoreWaitInfo waitInfo{};
		//		waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
		//		waitInfo.pNext = nullptr;
		//		waitInfo.flags = 0; 
		//		waitInfo.semaphoreCount = 1;
		//		waitInfo.pSemaphores = &m_timelineSemaphore[m_currentFrame];
		//		waitInfo.pValues = &waitvalue;
//
		//		// Wait forever until the semaphore reaches at least '5'
		//		VkResult result = vkWaitSemaphores(device, &waitInfo, UINT64_MAX);
//
		//		if (result != VK_SUCCESS) {
		//			throw std::runtime_error("Failed to wait on timeline semaphore!");
		//		}
		//	}
		//}

		m_currentFrame = ++m_currentFrame % LRenderTarget::MAX_FRAMES_IN_FLIGHT;

		//vkWaitForFences(device, 1, &m_fence[m_currentFrame], VK_TRUE, UINT64_MAX);
		//vkResetFences(device, 1, &m_fence[m_currentFrame]);
		m_passIndex = 0;
		VkResult result = vkAcquireNextImageKHR(device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, &m_activeSwapChainImageIndex);
		
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			Logger::Log(Logger::ERROR, "Failed to acquire next image: %d", result);
		}

		
		LSDLWindow::Render();	
	}
    
    bool LWindow::Init()
    {
    	LSDLWindow::Init();
    	allocationCallbacks = {};
    	allocationCallbacks.pfnAllocation = CustomAllocate;
    	allocationCallbacks.pfnReallocation = CustomReallocate;
    	allocationCallbacks.pfnFree = CustomFree;	
    	std::shared_ptr<LGraphicsContext> vulkanContext = GraphicsServices::GetContext();

    	if (!SDL_Vulkan_CreateSurface(GetSDLWindow(), vulkanContext->GetVkInstance(),&allocationCallbacks, &m_vkSurface)){
			CitrusCore::Logger::Log(CitrusCore::Logger::ERROR, "Failed to create vulkan surface [%s]", SDL_GetError());
			throw("Failed to create surface");
			return false;
		}

    	CreateSwapChain();	

		VkFenceCreateInfo fenceInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			//.flags = 0, // or VK_FENCE_CREATE_SIGNALED_BIT
			//.flags = 0,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
		};
		
		VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();

		for (int i = 0; i < LRenderTarget::MAX_FRAMES_IN_FLIGHT; ++i) {
			vkCreateFence(device, &fenceInfo, nullptr, &m_fence[i]);

			// We need a timeline as we have multiple submits per frame waiting on a single semaphore
			VkSemaphoreTypeCreateInfo timelineCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
				.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
				.initialValue = 0
			};

			VkSemaphoreCreateInfo semaphoreInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
			};
			
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]);

			VkSemaphoreCreateInfo timelinesemaphoreInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = &timelineCreateInfo,
				.flags = 0,
			};

			vkCreateSemaphore(device, &timelinesemaphoreInfo, nullptr, &m_timelineSemaphore[i]);
		}

    	return true;
    }

	uint64_t LWindow::GetPassIndexAndIncrement()
	{
		uint64_t value = m_passIndex;
		m_passIndex++; 
		return value;
	}

	uint64_t LWindow::GetFrameSemaphoreTimelineValueAndIncrement()
	{
		uint64_t value = m_currentSemaphoreValue;
		m_currentSemaphoreValue++; 
		return value;
	}

    void LWindow::CreateSwapChain() 
    {
    	std::shared_ptr<LGraphicsContext> vulkanContext = GraphicsServices::GetContext();
		vulkanContext->InitVulkanDevice(); // HACK
    	VkPhysicalDevice physicalDevice = vulkanContext->GetVulkanDevice().GetPhysicalDevice();	
    	VkDevice device = vulkanContext->GetVulkanDevice().GetVkDevice();	
    	uint32_t formatCount;
    	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_vkSurface, &formatCount, nullptr);

    	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);

    	if (formatCount != 0)
    	{
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_vkSurface, &formatCount, surfaceFormats.data());
    	}

		VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];

		for (const auto& format : surfaceFormats)
		{
			if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				surfaceFormat = format;
			}
		}

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_vkSurface, &capabilities);

		VkExtent2D extent;

		if (capabilities.currentExtent.width != UINT32_MAX) {
			extent = capabilities.currentExtent;
		} else {
			extent.width  = m_windowRect.Width;
			extent.height = m_windowRect.Height;
		}

		
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_vkSurface, &presentModeCount, nullptr);
    	std::vector<VkPresentModeKHR> presents(presentModeCount);
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

		if (presentModeCount != 0) {
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_vkSurface, &presentModeCount, presents.data());

			for (const auto& mode : presents) {
				if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
					presentMode = mode;
					break;
				}
			}
		}

		
		m_imageCount = capabilities.minImageCount + 1; 
		m_imageFormat = surfaceFormat.format;

    	VkSwapchainCreateInfoKHR createInfo{};
    	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    	createInfo.surface = m_vkSurface;
    	createInfo.minImageCount = m_imageCount;
    	createInfo.imageFormat = surfaceFormat.format;
    	createInfo.imageColorSpace = surfaceFormat.colorSpace;
    	createInfo.imageExtent = extent;
    	// imageArrayLayers - 1 Unless developing steroscopic 3D application, strokes beard.
    	createInfo.imageArrayLayers = 1;
		// Colour attachment -> thinking face. VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT for fullscreen pass 
    	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
	
		if (vkCreateSwapchainKHR(vulkanContext->GetVulkanDevice().GetVkDevice(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}		

		// Cache the swapchain images
		vkGetSwapchainImagesKHR(device, m_swapChain, &m_imageCount, nullptr);
		m_swapChainImages.resize(m_imageCount);
		vkGetSwapchainImagesKHR(device, m_swapChain, &m_imageCount, m_swapChainImages.data());
    }
}
