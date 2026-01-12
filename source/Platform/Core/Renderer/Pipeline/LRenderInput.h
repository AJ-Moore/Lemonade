#pragma once
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Platform/Core/Renderer/Lighting/Light.h>
#include <LCommon.h>

namespace Lemonade 
{
    struct LEMONADE_API LightPtr
    {
        const Lemonade::LightingData* LightPtr;
        uint32_t Count;
    };

    struct LEMONADE_API LRenderInput{
        std::shared_ptr<LUniformBuffer> LightBuffer = nullptr;
        LightPtr LightData;
    };
}