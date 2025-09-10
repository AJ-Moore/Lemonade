#pragma once 

#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/ASampler.h>
#include <vulkan/vulkan_core.h>

namespace Lemonade 
{
    class LEMONADE_API LSampler : public ASampler
    {
    public:
        LSampler() = default;
        virtual bool Init() override; 
        VkSampler GetSampler() const noexcept { return m_imageSampler; }
    private:
        VkSampler m_imageSampler = VK_NULL_HANDLE;
        bool m_bDoneInit = false;
        virtual void Unload() override;
    };
}