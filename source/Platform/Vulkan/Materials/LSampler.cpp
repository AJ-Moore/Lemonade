
#include <Platform/Vulkan/Materials/LSampler.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <vulkan/vulkan_core.h>

namespace Lemonade {
    bool LSampler::Init() 
    {
        VkDevice device = GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice();
        VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 8.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

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