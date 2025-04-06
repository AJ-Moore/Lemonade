#include <Platform/Vulkan/Renderer/Core/LGraphicsContext.h>

namespace Lemonade
{
    void LGraphicsContext::Unload()
    {
        vkDestroyInstance(m_vkInstance, nullptr);
    }

    bool LGraphicsContext::Init() 
    {
		VkApplicationInfo app = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO, // sType
			nullptr,                            // pNext
			m_windowCaption.c_str(),            // pApplicationName
			0,                                  // applicationVersion
			"Upsilon",				            // pEngineName
			0,                                  // engineVersion
			VK_API_VERSION_1_3                  // apiVersion
		};

        VkInstanceCreateInfo inst_info = {};
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		inst_info.pNext = nullptr;
		inst_info.flags = 0;
		inst_info.pApplicationInfo = &app;
		inst_info.enabledLayerCount = 0;
		inst_info.ppEnabledLayerNames = reinterpret_cast<const char* const*>(0);
		inst_info.enabledExtensionCount = 0;
		inst_info.ppEnabledExtensionNames = reinterpret_cast<const char* const*>("");

		vkCreateInstance(&inst_info, nullptr, &m_vkInstance);
    }
}
