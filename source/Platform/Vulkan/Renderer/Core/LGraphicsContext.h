#pragma once

#include <Platform/Core/Renderer/Core/AGraphicsContext.h>
#include <Platform/Vulkan/Renderer/LVulkanDevice.h>

namespace Lemonade
{
	class LEMONADE_API LGraphicsContext : public AGraphicsContext
	{
	public:
		const LVulkanDevice& GetVulkanDevice() const noexcept { return m_vulkanDevice; }
		const VkInstance& GetVkInstance() const { return m_vkInstance; }

		// Hack called prior to swap chain creation, we can probably mitigate this by creating a surface just to query properties?
		void InitVulkanDevice() { m_vulkanDevice.Init(); }
	protected:
		virtual void Unload();
		virtual bool Init();
		virtual void Update(){}
		virtual void Render(){}
	private: 
		LVulkanDevice m_vulkanDevice;
		VkInstance m_vkInstance = nullptr;
	};
}