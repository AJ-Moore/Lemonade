#include "Platform/Core/Renderer/RenderBlock/ARenderBlock.h"
#include <Platform/Core/Services/Services.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Util/Logger.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Platform/Vulkan/Renderer/LRenderBlock.h>
#include <LCommon.h>
#include <cstring>
#include <glm/fwd.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

#ifdef RENDERER_VULKAN
namespace Lemonade
{
	using CitrusCore::Logger;

	LRenderBlock::LRenderBlock() : ARenderBlock()
	{
		//SetName("LRenderBlock");
	}

	bool LRenderBlock::Init()
	{
		return true;
	}

	void LRenderBlock::Unload()
	{
		LVulkanDevice device = GraphicsServices::GetContext()->GetVulkanDevice();
		//vkDestroyBuffer(device.GetVkDevice(), m_vertexBuffer, nullptr);

		for (auto& vertexBuffer : m_vertexBuffers)
		{
			vkUnmapMemory(device.GetVkDevice(), vertexBuffer.second.VKDeviceMemory);
			vkDestroyBuffer(device.GetVkDevice(), vertexBuffer.second.Buffer, nullptr);
		}
	}

	void LRenderBlock::Update()
	{
		if (m_animationBufferDirty)
		{
			///dumpAnimationData();
			m_animationBufferDirty = false;
		}

		/// Dirty on mesh update
		if (m_bufferDirty)
		{
			DumpBufferData();
			m_bufferDirty = false;
		}
	}

	void LRenderBlock::Render()
	{
		if (m_mesh == nullptr || m_material == nullptr)
		{
			Logger::Log(Logger::ERROR, "Render block missing critical component, mesh or material.");
			return;
		}

		if (m_bufferDirty || m_mesh->GetVertices() == nullptr)
		{
			return;
		}

		LRenderTarget* renderTarget = static_cast<LRenderTarget*>(GraphicsServices::GetRenderer()->GetActiveRenderTarget());
		VkCommandBuffer commandBuffer = renderTarget->GetCommandBuffer();

		vkCmdBindVertexBuffers(commandBuffer, /*firstBinding=*/0, 
			static_cast<uint32_t>(m_vkBuffers.size()), 
			m_vkBuffers.data(), m_vkOffsets.data());

		int vertexCount = m_mesh->GetVertices() == nullptr ? 0 : m_mesh->GetVertices()->size();

		vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertexCount), 1, 0, 0);
	}

	void LRenderBlock::SetDrawMode(PrimitiveMode mode)
	{
		// TODO
		m_primitiveMode = (uint32)mode;
	}

	void LRenderBlock::SetVertexAttributes()
	{ 
	}

	void LRenderBlock::DumpAnimationData()
	{
	}

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;

		LVulkanDevice device = GraphicsServices::GetContext()->GetVulkanDevice();
		vkGetPhysicalDeviceMemoryProperties(device.GetPhysicalDevice(), &memProperties);
	
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
	
		throw std::runtime_error("Failed to find suitable memory type!");
	}

	void LRenderBlock::DumpBufferData()
	{
		if (m_mesh == nullptr)
		{
			Logger::Log(Logger::WARN, "Mesh not assigned!");
			return;
		}

		SetDrawMode(m_mesh->GetDrawMode());
		LVulkanDevice device = GraphicsServices::GetContext()->GetVulkanDevice();

		size_t vertexBufferSize = m_mesh->GetVertexBufferSize();
		size_t textureBufferSize = m_mesh->GetTextureBufferSize();
		size_t texture3DBufferSize = m_mesh->GetTexture3dBufferSize();
		size_t colourBufferSize = m_mesh->GetColourBufferSize();
		size_t normalBufferSize = m_mesh->GetNormalBufferSize();
		size_t tangentBufferSize = m_mesh->GetTangentBufferSize();
		size_t biTangentBufferSize = m_mesh->GetBiTangentBufferSize();
		size_t weightSize = m_mesh->GetWeightBufferSize();
		size_t boneIdSize = m_mesh->GetBoneIdBufferSize();

		int binding = 0;

		if (m_mesh->GetVertices() != nullptr)
		{
			m_vertexBuffers[VKBufferType::Position].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetVertices()->front()));
			m_vertexBuffers[VKBufferType::Position].DataSize = vertexBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::Position].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::Position].Binding = binding++;
		}

		if (m_mesh->GetNormals() != nullptr)
		{
			m_vertexBuffers[VKBufferType::Normal].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetNormals()->front()));
			m_vertexBuffers[VKBufferType::Normal].DataSize = normalBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::Normal].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::Normal].Binding = binding++;
		}

		if (m_mesh->GetUVS() != nullptr)
		{
			m_vertexBuffers[VKBufferType::UV].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetUVS()->front()));
			m_vertexBuffers[VKBufferType::UV].DataSize = textureBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::UV].Stride = sizeof(glm::vec2);
			m_vertexBuffers[VKBufferType::UV].Binding = binding++;
		}

		if (m_mesh->GetUVS3D() != nullptr)
		{
			m_vertexBuffers[VKBufferType::UV3D].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetUVS3D()->front()));
			m_vertexBuffers[VKBufferType::UV3D].DataSize = texture3DBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::UV3D].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::UV3D].Binding = binding++;
		}

		if (m_mesh->GetColours() != nullptr)
		{
			m_vertexBuffers[VKBufferType::Colour].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetColours()->front()));
			m_vertexBuffers[VKBufferType::Colour].DataSize = colourBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::Colour].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::Colour].Binding = binding++;
		}

		if (m_mesh->GetTangents() != nullptr)
		{
			m_vertexBuffers[VKBufferType::Tangents].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetTangents()->front()));
			m_vertexBuffers[VKBufferType::Tangents].DataSize = tangentBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::Tangents].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::Tangents].Binding = binding++;
		}

		if (m_mesh->GetBiTangents() != nullptr)
		{
			m_vertexBuffers[VKBufferType::BiTangents].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetBiTangents()->front()));
			m_vertexBuffers[VKBufferType::BiTangents].DataSize = biTangentBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BiTangents].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::BiTangents].Binding = binding++;
		}

		if (m_mesh->GetBoneWeights() != nullptr)
		{
			m_vertexBuffers[VKBufferType::BoneWeights].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetBoneWeights()->front()));
			m_vertexBuffers[VKBufferType::BoneWeights].DataSize = weightSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BoneWeights].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::BoneWeights].Binding = binding++;
		}

		if (m_mesh->GetBoneIds() != nullptr)
		{
			m_vertexBuffers[VKBufferType::BoneIds].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetBoneIds()->front()));
			m_vertexBuffers[VKBufferType::BoneIds].DataSize = boneIdSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BoneIds].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::BoneIds].Binding = binding++;
		}

		m_vkBuffers.clear();
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

		for (auto& vertexBuffer : m_vertexBuffers)
		{
			// Set binding desciption
			VkVertexInputBindingDescription bindingDescription;
			bindingDescription.binding = vertexBuffer.binding;
			bindingDescription.stride = vertexBuffer.stride;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputAttributeDescription attributeDescription;
			attributeDescription.binding = vertexBuffer.binding;
			attributeDescription.location = vertexBuffer.binding;
			attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescription.offset = 0;

			bindingDescriptions.push_back(bindingDescription);
			attributeDescriptions.push_back(attributeDescription);

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = vertexBuffer.second.DataSize;
			bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(device.GetVkDevice(), &bufferInfo, nullptr, &vertexBuffer.second.Buffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to create vertex buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device.GetVkDevice(), &vertexBuffer.second.Buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			if (vkAllocateMemory(device.GetVkDevice(), &allocInfo, nullptr, &vertexBuffer.second.VKDeviceMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate vertex buffer memory!");
			}

			vkBindBufferMemory(device.GetVkDevice(), vertexBuffer.second.Buffer, vertexBuffer.second.VKDeviceMemory, 0);

			vkMapMemory(device.GetVkDevice(), vertexBuffer.second.VKDeviceMemory, 0, vertexBuffer.second.DataSize, 0, &vertexBuffer.second.DataGPUMapped);
			std::memcpy(vertexBuffer.second.DataGPUMapped, vertexBuffer.second.DataCPUMapped, vertexBuffer.second.DataSize);

			m_vkBuffers.push_back(vertexBuffer.second.Buffer);
		}
	}
}
#endif
