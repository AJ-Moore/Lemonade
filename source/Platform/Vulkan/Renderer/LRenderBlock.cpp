#include "Platform/Vulkan/Materials/LSampler.h"
#include "Platform/Vulkan/Materials/Texture.h"
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Util/Logger.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Platform/Vulkan/Renderer/LRenderBlock.h>
#include <LCommon.h>
#include <algorithm>
#include <alloca.h>
#include <cstdint>
#include <cstring>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <Platform/Vulkan/Renderer/LShaderProgram.h>

#ifdef RENDERER_VULKAN
namespace Lemonade
{
	using CitrusCore::Logger;

	LRenderBlock::LRenderBlock() : ARenderBlock()
	{
		SetName("LRenderBlock");
	}

	bool LRenderBlock::Init()
	{
		m_uniformBuffers.resize(LRenderTarget::MAX_FRAMES_IN_FLIGHT);
		m_descriptorSets.resize(LRenderTarget::MAX_FRAMES_IN_FLIGHT);

		return true;
	}

	void LRenderBlock::Unload()
	{
		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
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

		int currentFrame = GraphicsServices::GetWindowManager()->GetActiveWindow()->GetCurrentFrame();

		LRenderTarget* renderTarget = static_cast<LRenderTarget*>(GraphicsServices::GetRenderer()->GetActiveRenderTarget());
		VkCommandBuffer commandBuffer = renderTarget->GetCommandBuffer();

		if (m_vkPipeline == VK_NULL_HANDLE)
		{
			CreateVkPipeline();
		}

		SetUniforms();
		
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_vkPipelineLayout,
			0, 1, &m_descriptorSets[currentFrame],
			0, nullptr);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);
		OnPipelineBound.Invoke(this);

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

		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
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
		const LVulkanDevice& device =GraphicsServices::GetContext()->GetVulkanDevice();

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
			m_vertexBuffers[VKBufferType::Position].Format = VK_FORMAT_R32G32B32_SFLOAT;
		}

		if (m_mesh->GetNormals() != nullptr)
		{
			m_vertexBuffers[VKBufferType::Normal].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetNormals()->front()));
			m_vertexBuffers[VKBufferType::Normal].DataSize = normalBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::Normal].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::Normal].Binding = binding++;
			m_vertexBuffers[VKBufferType::Normal].Format = VK_FORMAT_R32G32B32_SFLOAT;
		}

		if (m_mesh->GetUVS() != nullptr)
		{
			m_vertexBuffers[VKBufferType::UV].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetUVS()->front()));
			m_vertexBuffers[VKBufferType::UV].DataSize = textureBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::UV].Stride = sizeof(glm::vec2);
			m_vertexBuffers[VKBufferType::UV].Binding = binding++;
			m_vertexBuffers[VKBufferType::UV].Format = VK_FORMAT_R32G32_SFLOAT;
		}

		if (m_mesh->GetUVS3D() != nullptr)
		{
			m_vertexBuffers[VKBufferType::UV3D].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetUVS3D()->front()));
			m_vertexBuffers[VKBufferType::UV3D].DataSize = texture3DBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::UV3D].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::UV3D].Binding = binding++;
			m_vertexBuffers[VKBufferType::UV3D].Format = VK_FORMAT_R32G32B32_SFLOAT;
		}

		if (m_mesh->GetColours() != nullptr)
		{
			m_vertexBuffers[VKBufferType::Colour].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetColours()->front()));
			m_vertexBuffers[VKBufferType::Colour].DataSize = colourBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::Colour].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::Colour].Binding = binding++;
			m_vertexBuffers[VKBufferType::Colour].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		if (m_mesh->GetTangents() != nullptr)
		{
			m_vertexBuffers[VKBufferType::Tangents].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetTangents()->front()));
			m_vertexBuffers[VKBufferType::Tangents].DataSize = tangentBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::Tangents].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::Tangents].Binding = binding++;
			m_vertexBuffers[VKBufferType::Tangents].Format = VK_FORMAT_R32G32B32_SFLOAT;
		}

		if (m_mesh->GetBiTangents() != nullptr)
		{
			m_vertexBuffers[VKBufferType::BiTangents].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetBiTangents()->front()));
			m_vertexBuffers[VKBufferType::BiTangents].DataSize = biTangentBufferSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BiTangents].Stride = sizeof(glm::vec3);
			m_vertexBuffers[VKBufferType::BiTangents].Binding = binding++;
			m_vertexBuffers[VKBufferType::BiTangents].Format = VK_FORMAT_R32G32B32_SFLOAT;
		}

		if (m_mesh->GetBoneWeights() != nullptr)
		{
			m_vertexBuffers[VKBufferType::BoneWeights].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetBoneWeights()->front()));
			m_vertexBuffers[VKBufferType::BoneWeights].DataSize = weightSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BoneWeights].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::BoneWeights].Binding = binding++;
			m_vertexBuffers[VKBufferType::BoneWeights].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		if (m_mesh->GetBoneIds() != nullptr)
		{
			m_vertexBuffers[VKBufferType::BoneIds].DataCPUMapped = static_cast<void*>(glm::value_ptr(m_mesh->GetBoneIds()->front()));
			m_vertexBuffers[VKBufferType::BoneIds].DataSize = boneIdSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BoneIds].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::BoneIds].Binding = binding++;
			m_vertexBuffers[VKBufferType::BoneIds].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		m_vkBuffers.clear();
		m_attributeDescriptions.clear(); 
		m_bindingDescriptions.clear();

		//for (auto& vertexBuffer : m_vertexBuffers)
		for (VKBufferType bufferType : m_bufferOrder)
		{
			auto& vertexBuffer = m_vertexBuffers[bufferType];

			if (vertexBuffer.DataSize == 0)
			{
				// Skip buffer, not set 
				continue;
			}

			// Set binding desciption
			VkVertexInputBindingDescription bindingDescription;
			bindingDescription.binding = vertexBuffer.Binding;
			bindingDescription.stride = vertexBuffer.Stride;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputAttributeDescription attributeDescription;
			attributeDescription.binding = vertexBuffer.Binding;
			attributeDescription.location = vertexBuffer.Binding;
			attributeDescription.format = vertexBuffer.Format;
			attributeDescription.offset = 0;

			m_bindingDescriptions.push_back(bindingDescription);
			m_attributeDescriptions.push_back(attributeDescription);

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = vertexBuffer.DataSize;
			bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(device.GetVkDevice(), &bufferInfo, nullptr, &vertexBuffer.Buffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to create vertex buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device.GetVkDevice(), vertexBuffer.Buffer, &memRequirements);

			VkPhysicalDeviceMemoryProperties memProps;
			vkGetPhysicalDeviceMemoryProperties(device.GetPhysicalDevice(), &memProps);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			assert(allocInfo.memoryTypeIndex < memProps.memoryTypeCount);

			if (vkAllocateMemory(device.GetVkDevice(), &allocInfo, nullptr, &vertexBuffer.VKDeviceMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate vertex buffer memory!");
			}

			vkBindBufferMemory(device.GetVkDevice(), vertexBuffer.Buffer, vertexBuffer.VKDeviceMemory, 0);

			vkMapMemory(device.GetVkDevice(), vertexBuffer.VKDeviceMemory, 0, vertexBuffer.DataSize, 0, &vertexBuffer.DataGPUMapped);
			std::memcpy(vertexBuffer.DataGPUMapped, vertexBuffer.DataCPUMapped, vertexBuffer.DataSize);

			m_vkBuffers.push_back(vertexBuffer.Buffer);
			m_vkOffsets.push_back(0);
		}
	}

	void LRenderBlock::SetUniforms() 
	{
		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
		LCamera* activeCamera = GraphicsServices::GetRenderer()->GetActiveCamera();
		int currentFrame = GraphicsServices::GetWindowManager()->GetActiveWindow()->GetCurrentFrame();
		
		// Idiot allan we don't want to be doing this, clearly this was left from when we we're recreating descriptors each frame.
		//vkResetDescriptorPool(device.GetVkDevice(), m_vkDescriptorPool, 0);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffers[currentFrame].Buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(VertexData);


		std::vector<VkWriteDescriptorSet> writes;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[currentFrame];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		m_vertexData.cameraPosition = activeCamera->GetTransform()->GetPosition();
		m_vertexData.model = m_transform->GetWorldMatrix();         
        m_vertexData.view = activeCamera->GetViewMatrix();          
        m_vertexData.projection = activeCamera->GetProjMatrix();         
        m_vertexData.shadowPass = false;          
        m_vertexData.unlit = false;               
        m_vertexData.emission = 0;     
		writes.push_back(descriptorWrite);
		int sampleBindLocation = 0;

		for (auto& sampler : m_material->GetResource()->GetSamplers())
		{
			LSampler* nativeSampler = static_cast<LSampler*>(sampler.get());

			VkDescriptorImageInfo samplerDescriptor{
				.sampler = nativeSampler->GetSampler()
			};

			VkWriteDescriptorSet writeSampler = {};
			writeSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeSampler.dstSet = m_descriptorSets[currentFrame];
			writeSampler.dstBinding = 1;
			writeSampler.dstArrayElement = sampleBindLocation;
			writeSampler.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			writeSampler.descriptorCount = 1;
			writeSampler.pImageInfo = &samplerDescriptor;
			writes.push_back( writeSampler);
		}

		for (const auto& texture : m_material->GetResource()->GetTextures())
		{
			Texture* tex = static_cast<Texture*>(texture.second->GetTexture()->GetResource());
			uint32_t bindLocation = texture.second->GetBindLocation();

			// Imaage View
			VkDescriptorImageInfo imageDescriptor{
				.imageView   = tex->GetImageView(),
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};
			
			VkWriteDescriptorSet writeImage = {};
			writeImage.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeImage.dstSet = m_descriptorSets[currentFrame];
			writeImage.dstBinding = bindLocation;
			writeImage.dstArrayElement = 0;
			writeImage.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			writeImage.descriptorCount = 1;
			writeImage.pImageInfo = &imageDescriptor;

			writes.push_back(writeImage);
		}                

		std::memcpy(m_uniformBuffers[currentFrame].DataGPUMapped, m_uniformBuffers[currentFrame].DataCPUMapped, m_uniformBuffers[currentFrame].DataSize);
		vkUpdateDescriptorSets(device.GetVkDevice(), writes.size(), writes.data(), 0, nullptr);
	}

	void LRenderBlock::CreateVkDescriptors()
	{
		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();

		// Allocate Unifrom buffer memory 
		for (auto& uniformBuffer : m_uniformBuffers)
		{
			uniformBuffer.DataSize = sizeof(VertexData);

			if (uniformBuffer.Buffer == VK_NULL_HANDLE)
			{
				VkBufferCreateInfo bufferInfo{};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = uniformBuffer.DataSize;
				bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
				if (vkCreateBuffer(device.GetVkDevice(), &bufferInfo, nullptr, &uniformBuffer.Buffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to create uniform buffer!");
				}
			}
	

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device.GetVkDevice(), uniformBuffer.Buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			if (vkAllocateMemory(device.GetVkDevice(), &allocInfo, nullptr, &uniformBuffer.VKDeviceMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate vertex buffer memory!");
			}

			vkBindBufferMemory(device.GetVkDevice(), uniformBuffer.Buffer, uniformBuffer.VKDeviceMemory, 0);
			uniformBuffer.DataSize = sizeof(VertexData);
			vkMapMemory(device.GetVkDevice(), uniformBuffer.VKDeviceMemory, 0, uniformBuffer.DataSize, 0, &uniformBuffer.DataGPUMapped);
			uniformBuffer.DataCPUMapped = &m_vertexData;
			std::memcpy(uniformBuffer.DataGPUMapped, uniformBuffer.DataCPUMapped, uniformBuffer.DataSize);
		}

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1; 
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;   

		std::vector<VkDescriptorSetLayoutBinding> bindings = {
			uboLayoutBinding,
		};

		uint32_t bindingOffset = 1;
		int textureCount = 0;

		for (const auto& texture : m_material->GetResource()->GetTextures())
		{

			Texture* tex = static_cast<Texture*>(texture.second->GetTexture()->GetResource());
			uint32_t bindLocation = texture.second->GetBindLocation();

			VkDescriptorSetLayoutBinding imageLayoutBinding{};
			imageLayoutBinding.binding = bindingOffset + (bindLocation * textureCount);
			imageLayoutBinding.descriptorCount = 1;
			imageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			imageLayoutBinding.pImmutableSamplers = nullptr;
			imageLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		
			VkDescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding.binding = imageLayoutBinding.binding + 1; 
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			samplerLayoutBinding.pImmutableSamplers = nullptr;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			bindings.push_back(imageLayoutBinding);
			bindings.push_back(samplerLayoutBinding);
			textureCount++;
		}

		VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
		descriptorCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorCreateInfo.bindingCount = bindings.size();
		descriptorCreateInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device.GetVkDevice(), &descriptorCreateInfo, nullptr, &m_vkDescriptorSetLayout);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create descriptor set layout.");
		}

		VkDescriptorPoolSize poolSize[] = {
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = 100
            },
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = 100
            },
			{
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 100
            }
        };

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 3;
		poolInfo.pPoolSizes = poolSize;
		poolInfo.maxSets = 100;

		result = vkCreateDescriptorPool(device.GetVkDevice(), &poolInfo, nullptr, &m_vkDescriptorPool);
		
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool!");
		}

		std::vector<VkDescriptorSetLayout> layouts(LRenderTarget::MAX_FRAMES_IN_FLIGHT, m_vkDescriptorSetLayout); 

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr; 
		allocInfo.descriptorPool = m_vkDescriptorPool;
		allocInfo.descriptorSetCount = layouts.size();
		allocInfo.pSetLayouts = layouts.data();

		result = vkAllocateDescriptorSets(device.GetVkDevice(), &allocInfo, m_descriptorSets.data());
		
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate descriptor set!");
		}

		for (int i = 0; i < LRenderTarget::MAX_FRAMES_IN_FLIGHT; ++i) {
			if (m_descriptorSets[i] == VK_NULL_HANDLE) {
				throw std::runtime_error("Descriptor set handle is null!");
			}
		}
	}

	void LRenderBlock::CreateVkPipeline() 
	{
		CreateVkDescriptors();

		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
		LRenderTarget* renderTarget = static_cast<LRenderTarget*>(GraphicsServices::GetRenderer()->GetActiveRenderTarget());

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128;

		//std::vector<VkDescriptorSetLayout> layouts(LRenderTarget::MAX_FRAMES_IN_FLIGHT, m_vkDescriptorSetLayout); 

		m_vkPipelineLayoutCreateInfo = {};
		m_vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		m_vkPipelineLayoutCreateInfo.setLayoutCount = 1; 
		m_vkPipelineLayoutCreateInfo.pSetLayouts = &m_vkDescriptorSetLayout;
		//m_vkPipelineLayoutCreateInfo.setLayoutCount = layouts.size();
		//m_vkPipelineLayoutCreateInfo.pSetLayouts = layouts.data();
		m_vkPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		m_vkPipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		VkResult result = vkCreatePipelineLayout(device.GetVkDevice(), &m_vkPipelineLayoutCreateInfo, nullptr, &m_vkPipelineLayout);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkPipelineVertexInputStateCreateInfo vertexInput = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = (uint32_t)m_bindingDescriptions.size(),
			.pVertexBindingDescriptions = m_bindingDescriptions.data(),
			.vertexAttributeDescriptionCount = (uint32_t)m_attributeDescriptions.size(),
			.pVertexAttributeDescriptions = m_attributeDescriptions.data(),
		};

		LShaderProgram* shaderProgram = static_cast<LShaderProgram*>(m_material->GetResource()->GetShader().get());

		// Input Assembly 
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		VkPipelineViewportStateCreateInfo viewportState = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1
		};

		VkPipelineRasterizationStateCreateInfo rasterizer = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			//.cullMode = VK_CULL_MODE_BACK_BIT,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.lineWidth = 1.0f
		};

		LRenderTarget* activeTarget = static_cast<LRenderTarget*>(GraphicsServices::GetRenderer()->GetActiveRenderTarget());

		VkPipelineMultisampleStateCreateInfo multisampling = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = activeTarget->GetSampleCount(),
			.sampleShadingEnable = VK_FALSE
		};

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {
			.blendEnable = m_bBlendEnabled ? VK_TRUE : VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
							  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};
		
		VkPipelineColorBlendStateCreateInfo colorBlending = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.attachmentCount = activeTarget->GetAttachmentCount(),
			.pAttachments = &colorBlendAttachment
		};

		VkPipelineDepthStencilStateCreateInfo depthStencil = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE
		};

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		
		VkPipelineDynamicStateCreateInfo dynamicState = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 2,
			.pDynamicStates = dynamicStates
		};
		
		VkGraphicsPipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.pNext = nullptr; 
		createInfo.flags = 0;
		createInfo.stageCount = shaderProgram->GetShaderCreateInfo().size(); 
		createInfo.pStages = shaderProgram->GetShaderCreateInfo().data();
		createInfo.layout = m_vkPipelineLayout;
		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = -1;
		createInfo.renderPass = renderTarget->GetRenderPass();
		createInfo.pVertexInputState = &vertexInput;
		createInfo.pInputAssemblyState = &inputAssembly;
		createInfo.pViewportState = &viewportState;
		createInfo.pRasterizationState = &rasterizer;
		createInfo.pMultisampleState = &multisampling;
		createInfo.pColorBlendState = &colorBlending;
		createInfo.pDepthStencilState = &depthStencil;
		createInfo.pDynamicState = &dynamicState;

		result = vkCreateGraphicsPipelines(device.GetVkDevice(), 
			device.GetPipelineCache(), 
			1, 
			&createInfo, 
			nullptr, 
			&m_vkPipeline);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vkpipeline!");
		}
	}
}
#endif
