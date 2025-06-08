#include "Util/Logger.h"
#include <SDL3/SDL_vulkan.h>
#include <Platform/Vulkan/Renderer/Core/LGraphicsContext.h>

namespace Lemonade
{
    void LGraphicsContext::Unload()
    {
		m_vulkanDevice.Unload();
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

		//https://wiki.libsdl.org/SDL3/SDL_Vulkan_GetInstanceExtensions
		extern void handle_error(void);
		#ifndef VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		#define VK_EXT_DEBUG_REPORT_EXTENSION_NAME "VK_EXT_debug_report"
		#endif

		if (!SDL_Init(SDL_INIT_VIDEO)) {
			CitrusCore::Logger::Log(CitrusCore::Logger::ERROR, "Failed to initialise SDL.");
			return false;
		}

		//if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
		//{
		//	CitrusCore::Logger::Log(CitrusCore::Logger::ERROR, "Failed to load Vulkan library: %s", SDL_GetError());
		//	throw("Failed to load vulkan library.");
		//}

		Uint32 count_instance_extensions;
		const char * const *instance_extensions = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);

		if (instance_extensions == NULL) { return false; }

		int countExt = count_instance_extensions + 1;
		std::vector<const char*> extensions(countExt);
		extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
		SDL_memcpy(&extensions[1], instance_extensions, count_instance_extensions * sizeof(const char*)); 


        VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &app;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledExtensionCount = countExt;
		createInfo.ppEnabledExtensionNames = extensions.data();

		vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
		// NOT CALLED HERE -> We wait until a surface is created so we can query surface properties -> HACK 
		//m_vulkanDevice.Init();
		return true;
    }
}
