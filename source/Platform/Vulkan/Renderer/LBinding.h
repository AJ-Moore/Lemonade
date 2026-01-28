#pragma once
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/ABinding.h>
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Lemonade 
{
    class LEMONADE_API LBinding : public ABinding
    {
        friend class ARenderBlock;
    public:
        LBinding() = delete; 
        LBinding(uint32 bindLocation, VkDescriptorType type, VkShaderStageFlags shaderStage, uint32_t descriptorCount = 1) : 
            Lemonade::ABinding(bindLocation), 
            m_descriptorType(type), 
            m_shaderStageFlags(shaderStage),
            m_descriptorCount(descriptorCount){}

        const VkDescriptorSetLayoutBinding& GetLayoutBinding(){ return m_layoutBinding; }
        VkShaderStageFlags GetShaderStage() const { return m_shaderStageFlags; }
        VkDescriptorType GetDescriptorType() const { return m_descriptorType; }
        uint32_t GetDescriptorCount() const { return m_descriptorCount; }

    protected:
		virtual bool Init() override{ return true; }
		virtual void Unload() override{}
		virtual void Update() override{}
		virtual void Render() override{}
    private: 
        VkDescriptorSetLayoutBinding m_layoutBinding = {};
        VkDescriptorType m_descriptorType; 
        VkShaderStageFlags m_shaderStageFlags;
        uint32_t m_descriptorCount = 1;
    };
}