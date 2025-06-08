#pragma once

#include <LCommon.h>
#include <vulkan/vulkan_core.h>
#ifdef RENDERER_VULKAN
#include <Platform/Core/LObject.h>
#include <vector>

namespace Lemonade
{
	class LEMONADE_API LVulkanDevice : public LObject
	{
	public:
		LVulkanDevice();
		virtual ~LVulkanDevice();

		virtual bool Init() override;
		virtual void Unload() override;
		virtual void Update() override;
		virtual void Render() override;

		VkDevice GetVkDevice() const { return m_vkDevice; }
		VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
		VkQueue GetPresentationQueue() const { return m_presentationQueue; }
		VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
		VkQueue GetTransferQueue() const { return m_transferQueue; }
		VkQueue GetComputeQueue() const { return m_computeQueue; }
		VkCommandPool GetGraphicsCommandPool() const { return m_graphicsCommandPool; }
		VkPipelineCache GetPipelineCache() const { return m_vkCache; }

		int GetGraphicsQueueIndex() const { return m_graphicsQueueIndex; }
		int GetTransferQueueIndex() const { return m_transferQueueIndex; }
		int GetComputeQueueIndex() const { return m_computeQueueIndex; }
		int GetPresentationQueueIndex() const { return m_presentationQueueIndex; }

	private:
		bool CreateVulkanDevice();
		void CreatePipelineCache();
		void DestroyVulkanDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		int RateDeviceSuitability(VkPhysicalDevice device);
		int CreateDeviceQueue(VkDeviceQueueCreateInfo& queueCreateInfo, VkQueueFlagBits, bool presentation);

		bool m_graphicsIsPresentation = false;

		VkDevice m_vkDevice;
		VkPhysicalDevice m_physicalDevice;
		uint32_t m_queueFamilyCount = 0;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;

		VkQueue m_graphicsQueue;
		VkQueue m_transferQueue;
		VkQueue m_computeQueue;
		VkQueue m_presentationQueue;

		VkCommandPool m_graphicsCommandPool;
		VkCommandPool m_transferCommandPool;
		VkCommandPool m_computeCommandPool;

		VkPipelineCache m_vkCache;

		int m_graphicsQueueIndex = -1;
		int m_transferQueueIndex = -1;
		int m_computeQueueIndex = -1;
		int m_presentationQueueIndex = -1;
	};
}
#endif