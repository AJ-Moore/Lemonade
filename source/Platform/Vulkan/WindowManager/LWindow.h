#pragma once

#include <LCommon.h>
#ifdef RENDERER_VULKAN

#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Platform/Core/WindowManager/AWindow.h>

namespace Lemonade
{
	class LEMONADE_API LWindow : public AWindow {
	public:
		virtual ~LWindow(){}
		const VkSurfaceKHR& GetVkSurface() const { return m_vkSurface; }
	protected:
		virtual void Unload();
	private:
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		
	};
}

#endif