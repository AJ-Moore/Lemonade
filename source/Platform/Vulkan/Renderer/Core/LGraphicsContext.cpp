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
			"Game",            // pApplicationName
			0,                                  // applicationVersion
			"Lemonade",				            // pEngineName
			VK_MAKE_VERSION(0, 9, 0),                                  // engineVersion
			VK_API_VERSION_1_4             // apiVersion
		};

        VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &app;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledExtensionCount = 0;
		createInfo.ppEnabledExtensionNames = nullptr;

		vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
		return true;
    }
}
