#pragma once

#include <LCommon.h>
#ifdef RENDERER_VULKAN

#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Platform/Core/WindowManager/LSDLWindow.h>

namespace Lemonade
{
	class LEMONADE_API LWindow : public LSDLWindow {
	public:
		virtual ~LWindow(){}
	protected:
		virtual void Unload();
		virtual bool Init();
	private:
	};
}

#endif