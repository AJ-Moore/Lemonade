#include "Platform/Vulkan/Renderer/LVulkanDevice.h"
#include <LCommon.h>
#include <Util/Logger.h>

#ifdef RENDERER_VULKAN

//#include <vulkan/vk_enum_string_helper.h>
//#include <vulkan/vulkan_to_string.hpp>
#include <Platform/Core/Services/Services.h>

using Logger = CitrusCore::Logger;

namespace Lemonade
{
	LVulkanDevice::LVulkanDevice()
	{
	}

	LVulkanDevice::~LVulkanDevice()
	{
		DestroyVulkanDevice();
	}

	bool LVulkanDevice::Init()
	{
		if (!CreateVulkanDevice())
		{
			Logger::Log(Logger::ERROR, "Failed to create Vulkan device!");
			return false;
		}
		return true;
	}

	void LVulkanDevice::Unload()
	{
	}

	void LVulkanDevice::Update()
	{
	}

	void LVulkanDevice::Render() 
	{
	}

	int LVulkanDevice::CreateDeviceQueue(VkDeviceQueueCreateInfo& queueCreateInfo, VkQueueFlagBits queue, bool presentation = false)
	{
		VkBool32 supportsPresent = false;
		bool checkPresentation = false;
		const VkSurfaceKHR& surface = Services::GetWindowManager()->GetMainWindow()->GetVkSurface();

		if (queue == VK_QUEUE_GRAPHICS_BIT)
		{
			//checkPresentation = true;
		}

		uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
		for (uint32_t i = 0; i < m_queueFamilyCount; ++i)
		{
			if (m_queueFamilyProperties[i].queueFlags & queue || presentation)
			{
				if (checkPresentation || presentation)
				{
					vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, surface, &supportsPresent);

					if (supportsPresent == VK_TRUE)
					{
						m_graphicsIsPresentation = true;
						graphicsQueueFamilyIndex = i;
						break;
					}
					else
					{
						graphicsQueueFamilyIndex = i;
						continue;
					}
				}

				graphicsQueueFamilyIndex = i;
				break;
			}
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr; // no extensions.
		queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = new float[1]{ 1.0f };
		queueCreateInfo.flags = 0;

		return graphicsQueueFamilyIndex;
	}

	bool LVulkanDevice::CreateVulkanDevice()
	{
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		uint32_t deviceCount = 0;	
		const VkInstance& instance = Services::GetContext()->GetVkInstance();
		//const VkSurfaceKHR& surface = UServiceLocator::getInstance()->getWindowManager()->getMainWindow()->getVkSurface();

		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
			return false;
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		int bestScore = 0;

		for (const auto& device : devices)
		{
			int score = RateDeviceSuitability(device);

			if (score > bestScore) {
				bestScore = score;
				physicalDevice = device;
			}
		}

		m_physicalDevice = physicalDevice;

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &m_queueFamilyCount, nullptr);
		m_queueFamilyProperties.resize(m_queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &m_queueFamilyCount, m_queueFamilyProperties.data());

		VkDeviceQueueCreateInfo queueCreateInfo[4];

		m_graphicsQueueIndex = CreateDeviceQueue(queueCreateInfo[0], VK_QUEUE_GRAPHICS_BIT);
		m_computeQueueIndex = CreateDeviceQueue(queueCreateInfo[1], VK_QUEUE_COMPUTE_BIT);
		m_transferQueueIndex = CreateDeviceQueue(queueCreateInfo[2], VK_QUEUE_TRANSFER_BIT);

		m_presentationQueueIndex = CreateDeviceQueue(queueCreateInfo[3], VK_QUEUE_TRANSFER_BIT, true);

		// Enabled Extensions
		const char* enabledExtensions[] =
		{
			"VK_KHR_swapchain",
		};

		// Enabled Layers
		const char* enabledLayers[] =
		{
			"VK_LAYER_RENDERDOC_Capture",
		};

		// Get supported features
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &supportedFeatures);

		//VkPhysicalDeviceFeatures deviceFeatures = {};
		//deviceFeatures.multiDrawIndirect = supportedFeatures.multiDrawIndirect;
		//deviceFeatures.geometryShader = VK_TRUE;
		//deviceFeatures.tessellationShader = VK_TRUE;


		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfo;
		deviceCreateInfo.pEnabledFeatures = &supportedFeatures;
		deviceCreateInfo.enabledExtensionCount = 1;
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions;
		deviceCreateInfo.enabledLayerCount = 1;
		deviceCreateInfo.ppEnabledLayerNames = enabledLayers;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.pNext = nullptr;


		VkResult result = vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice);

		if (result != VK_SUCCESS) {
			//CitrusCore::Logger::Log(CitrusCore::Logger::ERROR,"Vulkan Error: %s", string_VkResult(result));
			throw std::runtime_error("failed to create logical device!");
			return false;
		}

		vkGetDeviceQueue(m_vkDevice, queueCreateInfo[0].queueFamilyIndex, 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_vkDevice, queueCreateInfo[1].queueFamilyIndex, 1, &m_computeQueue);
		vkGetDeviceQueue(m_vkDevice, queueCreateInfo[2].queueFamilyIndex, 2, &m_transferQueue);
		vkGetDeviceQueue(m_vkDevice, queueCreateInfo[3].queueFamilyIndex, 3, &m_presentationQueue);

		// Create command pools
		VkCommandPoolCreateInfo graphicsPoolInfo = {};
		graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		graphicsPoolInfo.queueFamilyIndex = queueCreateInfo[0].queueFamilyIndex;
		graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_vkDevice, &graphicsPoolInfo, nullptr, &m_graphicsCommandPool) != VK_SUCCESS) {
			CitrusCore::Logger::Log(CitrusCore::Logger::ERROR, "Failed to create graphics command pool!");
			throw std::runtime_error("failed to create graphics command pool!");
			return false;
		}

		//VkBool32 supportsPresent = false;
		//int graphicsQueueFamilyIndex = -1;
		//
		//// Get presentation queue
		//for (uint32_t i = 0; i < m_queueFamilyCount; i++)
		//{
		//	VkBool32 supportsPresent;
		//	vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, surface, &supportsPresent);
		//
		//	if ((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supportsPresent) {
		//		graphicsQueueFamilyIndex = i;
		//	}
		//}

		return true;
	}

	void LVulkanDevice::DestroyVulkanDevice()
	{
		vkDestroyDevice(m_vkDevice, nullptr);
	}

	bool LVulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader;
	}

	int LVulkanDevice::RateDeviceSuitability(VkPhysicalDevice device)
	{
		int score = 0;

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		// Application can't function without geometry shaders
		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		return score;
	}
}

#endif
