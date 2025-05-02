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