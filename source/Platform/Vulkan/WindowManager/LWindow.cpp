#include <Platform/Vulkan/WindowManager/LWindow.h>

namespace Lemonade 
{
    void LWindow::Unload()
    {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
    }
}