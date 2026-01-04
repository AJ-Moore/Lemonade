#include <Platform/Vulkan/Renderer/LUniformBuffer.h>

namespace Lemonade
{
    bool LUniformBuffer::Init()
    {
        m_buffers.resize(LRenderTarget::MAX_FRAMES_IN_FLIGHT);
        return true;
    }

    void LUniformBuffer::UpdateDescriptors(VkDescriptorSet dstSet, uint32_t bindLocation, bool write)
    {
        uint32_t currentFrame = GraphicsServices::GetWindowManager()->GetActiveWindow()->GetCurrentFrame();
        LVKBuffer& buffer = GetLVKBuffer(currentFrame);

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer.Buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = buffer.DataSize;

        m_writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        m_writeDescriptorSet.dstSet = dstSet;
        m_writeDescriptorSet.dstBinding = bindLocation;
        m_writeDescriptorSet.dstArrayElement = 0;
        m_writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        m_writeDescriptorSet.descriptorCount = 1;
        m_writeDescriptorSet.pBufferInfo = &bufferInfo;

        if (write)
        {
            const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
            vkUpdateDescriptorSets(device.GetVkDevice(), 1, &m_writeDescriptorSet, 0, nullptr);
        }
    }

    void LUniformBuffer::DumpBuffer()
    {
        uint32_t currentFrame = GraphicsServices::GetWindowManager()->GetActiveWindow()->GetCurrentFrame();
        LVKBuffer& buffer = GetLVKBuffer(currentFrame);

        if (buffer.Dirty || m_bAlwaysUpdate)
        {
            buffer.DataCPUMapped = static_cast<void*>(this->GetData());
            buffer.DataSize = GetSize();
            std::memcpy(buffer.DataGPUMapped, buffer.DataCPUMapped, buffer.DataSize);
        }
    }

    void LUniformBuffer::SetDirty()
    {
        for (auto& buffer : m_buffers)
        {
            buffer.Dirty = true;
        }
    }
}