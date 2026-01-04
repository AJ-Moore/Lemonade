#pragma once
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Vulkan/Renderer/LVKBuffer.h>
#include <Platform/Core/Renderer/Pipeline/AUniformBuffer.h>
#include <Platform/Core/LObject.h>
#include <LCommon.h>

namespace Lemonade 
{
    class LEMONADE_API LUniformBuffer : AUniformBuffer
    {
    public:
        LUniformBuffer() = delete; 
        LUniformBuffer(void* dataPtr, uint32_t size) : Lemonade::AUniformBuffer(dataPtr, size){}

        void DumpBuffer();
        void UpdateDescriptors(VkDescriptorSet dstSet, uint32_t bindLocation, bool write = false);
        LVKBuffer& GetLVKBuffer(uint32_t frame) { return m_buffers.at(frame); }

        void SetDirty();
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
        VkWriteDescriptorSet m_writeDescriptorSet;
    };
}