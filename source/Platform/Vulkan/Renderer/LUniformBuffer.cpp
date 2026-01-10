#include <Platform/Core/Renderer/Pipeline/AUniformBuffer.h>
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <vulkan/vulkan_core.h>

namespace Lemonade
{
    bool LUniformBuffer::Init()
    {
        m_buffers.resize(LRenderTarget::MAX_FRAMES_IN_FLIGHT);

        const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = GetSize();

        if (GetBufferType() == LBufferType::Uniform)
        {
            bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            m_descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        else {
            bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            m_descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }

        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        for (int i = 0; i < m_buffers.size(); ++i)
        {
            if (vkCreateBuffer(device.GetVkDevice(), &bufferInfo, nullptr, &m_buffers[i].Buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create uniform buffer!");
            }
        }

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
        m_writeDescriptorSet.descriptorType = GetBufferType() == LBufferType::Uniform ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
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