#pragma once

#include <Core/Renderer/Core/AGraphicsContext.h>#
#include <Vulkan/Renderer/LVulkanDevice.h>

namespace Lemonade
{
	class LEMONADE_API LGraphicsContext : public AGraphicsContext
	{
		const LVulkanDevice& GetVulkanDevice() const noexcept { return m_vulkanDevice; }
	private: 
		LVulkanDevice m_vulkanDevice;
	};
}