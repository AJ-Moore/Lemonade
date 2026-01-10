#pragma once
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Vulkan/Renderer/LVKBuffer.h>
#include <Platform/Core/Renderer/Pipeline/AUniformBuffer.h>
#include <Platform/Core/LObject.h>
#include <LCommon.h>
#include <vulkan/vulkan_core.h>

namespace Lemonade 
{
    class LEMONADE_API LUniformBuffer : public AUniformBuffer
    {
    public:
        LUniformBuffer() = delete; 
        LUniformBuffer(LBufferType type, void* dataPtr, uint32_t size) : Lemonade::AUniformBuffer(type, dataPtr, size){}

        void UpdateBuffer(void* data, uint32_t size){/*Tdododo*/}
        void DumpBuffer();
        void UpdateDescriptors(VkDescriptorSet dstSet, uint32_t bindLocation, bool write = false);
        LVKBuffer& GetLVKBuffer(uint32_t frame) { return m_buffers.at(frame); }
        VkWriteDescriptorSet GetWrite() { return m_writeDescriptorSet; }
        void SetDirty();
        void SetShaderStage(VkShaderStageFlags flags) { m_shaderStageFlags = flags; }
        VkShaderStageFlags GetShaderStage() const { return m_shaderStageFlags; }
        VkDescriptorType GetDescriptorType(){ return m_descriptorType; }
    protected:
		virtual bool Init() override;
		virtual void Unload() override{}
		virtual void Update() override{}
		virtual void Render() override{}
    private: 
        /// Whether the buffer data is copied every time we call update descriptors.
        bool m_bAlwaysUpdate = false;
        bool m_bDoneInit = false;
        std::vector<LVKBuffer> m_buffers;
        VkDescriptorType m_descriptorType;
        VkWriteDescriptorSet m_writeDescriptorSet;
        VkShaderStageFlags m_shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    };
}