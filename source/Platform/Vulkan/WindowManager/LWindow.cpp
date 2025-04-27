#include "SDL3/SDL_vulkan.h"
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <SDL3/SDL_vulkan.h>

namespace Lemonade 
{
    void LWindow::Unload()
    {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
    }
    
    bool LWindow::Init()
    {
		SDL_Vulkan_CreateSurface(GetSDLWindow(), m_vkInstance, &m_vkSurface);
    }
}