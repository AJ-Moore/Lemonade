#pragma once

#include <LCommon.h>
#ifdef RENDERER_VULKAN

#include <Vulkan/Renderer/LVulkanDevice.h>
#include <Core/WindowManager/AWindowManager.h>
#include <Core/WindowManager/AWindow.h>

namespace Lemonade
{
	class LEMONADE_API LWindow : public AWindow {
	public:
		virtual ~LWindowManager();
		LVulkanDevice* GetVulkanDevice() { return &m_vulkanDevice; }
	private:
		LVulkanDevice m_vulkanDevice;
	};
}

#endif