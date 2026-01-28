#include <Util/Logger.h>
#include <SDL3/SDL_vulkan.h>
#include <Platform/Vulkan/Renderer/Core/LGraphicsContext.h>

namespace Lemonade
{
	using CitrusCore::Logger;

    void LGraphicsContext::Unload()
    {
		m_vulkanDevice.Unload();
        vkDestroyInstance(m_vkInstance, nullptr);
    }

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		// Optional: filter messages if needed
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			Logger::Log(Logger::ERROR, "[Vulkan] %s", pCallbackData->pMessage);
		}
	
		return VK_FALSE; // don't abort
	}

    bool LGraphicsContext::Init() 
    {
		VkApplicationInfo app = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO, // sType
			nullptr,                            // pNext
			"Game",            // pApplicationName
			0,                                  // applicationVersion
			"Lemonade",				            // pEngineName
			VK_MAKE_VERSION(0, 9, 0),     // engineVersion
			VK_API_VERSION_1_4             // apiVersion
		};

		//https://wiki.libsdl.org/SDL3/SDL_Vulkan_GetInstanceExtensions
		extern void handle_error(void);
		#ifndef VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		#define VK_EXT_DEBUG_REPORT_EXTENSION_NAME "VK_EXT_debug_report"
		#endif

		if (!SDL_Init(SDL_INIT_VIDEO)) {
			CitrusCore::Logger::Log(CitrusCore::Logger::ERROR, "Failed to initialise SDL. [%s]", SDL_GetError());
			return false;
		}

		Uint32 count_instance_extensions;
		const char * const *instance_extensions = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);

		if (instance_extensions == NULL) { return false; }

		int countExt = count_instance_extensions + 2;
		std::vector<const char*> extensions(countExt);
		extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
		extensions[1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
		SDL_memcpy(&extensions[2], instance_extensions, count_instance_extensions * sizeof(const char*)); 

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = 
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;
		debugCreateInfo.pUserData = nullptr;

		VkValidationFeatureDisableEXT disables[] = {
			VK_VALIDATION_FEATURE_DISABLE_UNIQUE_HANDLES_EXT
		};
		
		VkValidationFeaturesEXT validationFeatures = {
			.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
			.disabledValidationFeatureCount = 1,
			.pDisabledValidationFeatures = disables
		};

        VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = &validationFeatures;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &app;
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
		// NOT CALLED HERE -> We wait until a surface is created so we can query surface properties -> HACK 
		//m_vulkanDevice.Init();
		return true;
    }
}
