#pragma once

#include <LCommon.h>
#ifdef RENDERER_VULKAN

#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Platform/Core/WindowManager/LSDLWindow.h>

namespace Lemonade
{
	class LEMONADE_API LWindow : public LSDLWindow {
		friend class LWindowManager; 
	public:
		virtual ~LWindow(){}
		const VkSurfaceKHR& GetVkSurface() const { return m_vkSurface; }
	protected:
		virtual void Unload();
		virtual bool Init();
	private:
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkAllocationCallbacks allocationCallbacks;		
	};
}

#endif