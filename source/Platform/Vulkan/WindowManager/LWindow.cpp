#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/Core/LGraphicsContext.h>
#include "Platform/Core/WindowManager/LSDLWindow.h"
#include "SDL3/SDL_vulkan.h"
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include <vector>

namespace Lemonade 
{
    // Custom allocation functions
    void* CustomAllocate(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
      return malloc(size);  // Example: just use malloc for allocations
    }

    void* CustomReallocate(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
      return realloc(pOriginal, size);  // Example: just use realloc for reallocations
    }

    void CustomFree(void* pUserData, void* pMemory) {
      free(pMemory);  // Example: just use free for deallocations
    }

    void LWindow::Unload()
    {
        std::shared_ptr<LGraphicsContext> vulkanContext = GraphicsServices::GetContext();
        vkDestroySurfaceKHR(vulkanContext->GetVkInstance(), m_vkSurface, nullptr);
    }
    
    bool LWindow::Init()
    {
      LSDLWindow::Init();
      allocationCallbacks = {};
      allocationCallbacks.pfnAllocation = CustomAllocate;
      allocationCallbacks.pfnReallocation = CustomReallocate;
      allocationCallbacks.pfnFree = CustomFree;

      std::shared_ptr<LGraphicsContext> vulkanContext = GraphicsServices::GetContext();
      SDL_Vulkan_CreateSurface(GetSDLWindow(), vulkanContext->GetVkInstance(),&allocationCallbacks, &m_vkSurface);
      return true;
    }
}