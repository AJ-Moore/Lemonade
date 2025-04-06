#pragma once
#include <LCommon.h>
#ifdef RENDERER_VULKAN

#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Core/WindowManager/AWindowManager.h>

namespace Lemonade
{
	class LEMONADE_API LWindowManager : public LWindowManager {
	public:
		virtual ~LWindowManager();
		LVulkanDevice* GetVulkanDevice() { return &m_vulkanDevice; }
	private:
		LVulkanDevice m_vulkanDevice;
	};
}

#endif