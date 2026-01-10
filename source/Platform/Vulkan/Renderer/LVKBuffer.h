#pragma once
#include <LCommon.h>

#ifdef RENDERER_VULKAN
#include <cstddef>
#include <vulkan/vulkan_core.h>

namespace Lemonade{
    struct LEMONADE_API LVKBuffer
    {
        VkBuffer Buffer;
        void* DataCPUMapped = nullptr;
        void* DataGPUMapped = nullptr;
        size_t DataSize = 0;
        VkDeviceMemory VKDeviceMemory;
        size_t Stride;
        int Binding = 0;
        VkFormat Format = VK_FORMAT_R32G32B32_SFLOAT;
        bool Dirty = true;
    };
}
#endif 