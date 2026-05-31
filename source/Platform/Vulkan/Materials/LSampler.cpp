#include "Platform/Core/Renderer/Materials/ASampler.h"
#include <Platform/Vulkan/Materials/LSampler.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <vulkan/vulkan_core.h>

namespace Lemonade {
    bool LSampler::Init() 
    {
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = GetSamplerInfo().MagFilter == LTextureFilter::Linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		samplerInfo.minFilter = GetSamplerInfo().MinFilter == LTextureFilter::Linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 8.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.mipmapMode = GetSamplerInfo().MipMapMode == SamplerMipmapMode::Linear ?  VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkResult result = vkCreateSampler(device, &samplerInfo, nullptr, &m_imageSampler);
        
        if (result != VK_SUCCESS)
        {
            throw("Create sampler failed.");
        }

        m_bDoneInit = true;
        return true;
    }

    void LSampler::Unload() 
    {
        if (m_imageSampler == VK_NULL_HANDLE)
        {
            return;
        }

        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        vkDestroySampler(device, m_imageSampler, nullptr);
    }
}