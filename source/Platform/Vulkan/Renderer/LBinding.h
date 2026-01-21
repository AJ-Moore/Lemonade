#pragma once
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/ABinding.h>
#include <vulkan/vulkan_core.h>

namespace Lemonade 
{
    class LEMONADE_API LBinding : public ABinding
    {
        friend class ARenderBlock;
    public:
        LBinding() = delete; 
        LBinding(uint32 bindLocation, VkDescriptorType type, VkShaderStageFlags shaderStage) : 
            Lemonade::ABinding(bindLocation), 
            m_descriptorType(type), 
            m_shaderStageFlags(shaderStage){}

        const VkDescriptorSetLayoutBinding& GetLayoutBinding(){ return m_layoutBinding; }
        VkShaderStageFlags GetShaderStage() const { return m_shaderStageFlags; }
        VkDescriptorType GetDescriptorType(){ return m_descriptorType; }
    protected:
		virtual bool Init() override{ return true; }
		virtual void Unload() override{}
		virtual void Update() override{}
		virtual void Render() override{}
    private: 
        VkDescriptorSetLayoutBinding m_layoutBinding = {};
        VkDescriptorType m_descriptorType; 
        VkShaderStageFlags m_shaderStageFlags;
    };
}