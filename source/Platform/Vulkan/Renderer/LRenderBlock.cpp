#include <Platform/Core/Renderer/Materials/Material.h>
#include <Platform/Core/Renderer/Materials/TextureType.h>
#include <Platform/Vulkan/Materials/LSampler.h>
#include <Platform/Vulkan/Materials/Texture.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Util/Logger.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LVulkanDevice.h>
#include <Platform/Vulkan/Renderer/LRenderBlock.h>
#include <LCommon.h>
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

	bool LRenderBlock::AddUniformBuffer(std::shared_ptr<LUniformBuffer> buffer)
	{
		return m_uniformBuffers.Add(buffer->GetUID(), buffer); 
	}

	bool LRenderBlock::Init()
	{
		m_vertexDataUniformBuffers.resize(LRenderTarget::MAX_FRAMES_IN_FLIGHT);
		m_boneMatBuffer.resize(LRenderTarget::MAX_FRAMES_IN_FLIGHT);
		m_descriptorSets.resize(LRenderTarget::MAX_FRAMES_IN_FLIGHT);
		
		// 1x1 Colors for quiet defaults
		Colour defaultWhite   = { 255, 255, 255, 255 };  // BaseColor / diffuse
		Colour defaultFlatNormal = { 128, 128, 255, 255 }; // Normal map (flat)
		Colour defaultORM     = { 255, 255, 0, 255 };    // Roughness (G)=1, Metallic(B)=0, AO(R)=1
		Colour defaultBlack   = { 0, 0, 0, 255 };        // Emissive / optional Metalness

		// Load defaults
		m_defaultDiffuse.LoadNativeTextureFromPixels(&defaultWhite, 1, 1);
		m_defaultRoughness.LoadNativeTextureFromPixels(&defaultORM, 1, 1); 
		m_defaultNormal.LoadNativeTextureFromPixels(&defaultFlatNormal, 1, 1);
		m_defaultAo.LoadNativeTextureFromPixels(&defaultORM, 1, 1); 
		m_defaultEmission.LoadNativeTextureFromPixels(&defaultBlack, 1, 1);
		m_defaultMetalness.LoadNativeTextureFromPixels(&defaultORM, 1, 1);

		m_defaultTextures[TextureType::Diffuse] = &m_defaultDiffuse;
		m_defaultTextures[TextureType::Normal]            = &m_defaultNormal;     
		m_defaultTextures[TextureType::Roughness]         = &m_defaultRoughness;      
		m_defaultTextures[TextureType::AmbientOcclusion]  = &m_defaultAo;     
		m_defaultTextures[TextureType::Emissive]          = &m_defaultEmission;    
		m_defaultTextures[TextureType::Metalness]         = &m_defaultMetalness;    

		return true;
	}

	void LRenderBlock::Unload()
	{
		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
		int currentFrame = GraphicsServices::GetWindowManager()->GetActiveWindow()->GetCurrentFrame();

		for (auto& vertexBuffer : m_vertexBuffers)
		{
			vkUnmapMemory(device.GetVkDevice(), vertexBuffer.second.VKDeviceMemory);
			vkDestroyBuffer(device.GetVkDevice(), vertexBuffer.second.Buffer, nullptr);
		}

		vkUnmapMemory(device.GetVkDevice(), m_boneMatBuffer[currentFrame].VKDeviceMemory);
		vkDestroyBuffer(device.GetVkDevice(), m_boneMatBuffer[currentFrame].Buffer, nullptr);
	}

	void LRenderBlock::Update()
	{
		if (m_animationBufferDirty)
		{
			//DumpAnimationData();
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
	
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);
		OnPipelineBound.Invoke(this);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_vkPipelineLayout,
			0, 1, &m_descriptorSets[currentFrame],
			0, nullptr);

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
		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();
		int currentFrame = GraphicsServices::GetWindowManager()->GetActiveWindow()->GetCurrentFrame();

		LVKBuffer boneBuffer = m_boneMatBuffer[currentFrame];

		// cpy mem here
		boneBuffer.DataCPUMapped = static_cast<void*>(m_mesh->GetBoneMatrix().get()->data());
		std::memcpy(boneBuffer.DataGPUMapped, boneBuffer.DataCPUMapped, boneBuffer.DataSize);
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

		if (m_mesh->GetWeightBufferSize() > 0)
		{
			m_vertexBuffers[VKBufferType::BoneWeights].DataCPUMapped = static_cast<void*>(m_mesh->GetBoneWeights()->data());
			m_vertexBuffers[VKBufferType::BoneWeights].DataSize = weightSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BoneWeights].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::BoneWeights].Binding = binding++;
			m_vertexBuffers[VKBufferType::BoneWeights].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		if (m_mesh->GetBoneIdBufferSize() > 0)
		{
			m_vertexBuffers[VKBufferType::BoneIds].DataCPUMapped = static_cast<void*>(m_mesh->GetBoneIds()->data());
			m_vertexBuffers[VKBufferType::BoneIds].DataSize = boneIdSize * sizeof(float);
			m_vertexBuffers[VKBufferType::BoneIds].Stride = sizeof(glm::vec4);
			m_vertexBuffers[VKBufferType::BoneIds].Binding = binding++;
			m_vertexBuffers[VKBufferType::BoneIds].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		m_vkBuffers.clear();
		m_attributeDescriptions.clear(); 
		m_bindingDescriptions.clear();

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
			allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

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

		// Vertex data - uniform buffer
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_vertexDataUniformBuffers[currentFrame].Buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(VertexData);

		// Bone matrices(animation) - storage buffer 
		LVKBuffer boneBuffer = m_boneMatBuffer[currentFrame];
		VkDescriptorBufferInfo boneBufferInfo{};
		boneBufferInfo.buffer = m_boneMatBuffer[currentFrame].Buffer;
		boneBufferInfo.offset = 0;
		boneBufferInfo.range = m_boneMatBuffer[currentFrame].DataSize;

		std::vector<VkWriteDescriptorSet> writes;
		int bindLocation = 0;

		VkWriteDescriptorSet uniformBufferWrite{};
		uniformBufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		uniformBufferWrite.dstSet = m_descriptorSets[currentFrame];
		uniformBufferWrite.dstBinding = bindLocation++;
		uniformBufferWrite.dstArrayElement = 0;
		uniformBufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferWrite.descriptorCount = 1;
		uniformBufferWrite.pBufferInfo = &bufferInfo;

		m_vertexData.cameraPosition = activeCamera->GetTransform()->GetWorldPosition();
		m_vertexData.model = m_transform->GetWorldMatrix();         
        m_vertexData.view = activeCamera->GetViewMatrix();          
        m_vertexData.projection = activeCamera->GetProjMatrix();         
        m_vertexData.shadowPass = false;          
        m_vertexData.unlit = false;               
        m_vertexData.emission = 0;     
		m_vertexData.baseColour = m_material->GetResource()->GetBaseColour();
		writes.push_back(uniformBufferWrite);
		int sampleBindLocation = 0;

		// Write bones
		if (m_mesh->GetBoneCount())
		{
			VkWriteDescriptorSet boneDescriptor{};
			boneDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			boneDescriptor.dstSet = m_descriptorSets[currentFrame];
			boneDescriptor.dstBinding = bindLocation++;
			boneDescriptor.dstArrayElement = 0;
			boneDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			boneDescriptor.descriptorCount = 1;
			boneDescriptor.pBufferInfo = &boneBufferInfo;
			writes.push_back(boneDescriptor);
		}

		// Think twice this is to keep what would otherwise be locally allocated variables alive until  we update the descriptor set.
		std::vector<VkDescriptorImageInfo> imageInfos;

		int samplerCount = m_material->GetResource()->GetSamplers().size();
		int textureCount = m_material->GetResource()->GetTextures().size();

		// reserve to prevent segfault, we rely on pointers that may otherwise change when vector is resized behind the scenes
		imageInfos.reserve(samplerCount + textureCount);

		LSampler* defaultSampler = static_cast<LSampler*>(m_material->GetResource()->GetSamplers().begin()->get());
		
		// Needs improvement -> prevents us unecessarily updating the textures every frame.
		// Bug here currently that causes flashing -> needs investigation
		//if (m_bTextureSamplersDirty)
		{
			for (auto& sampler : m_material->GetResource()->GetSamplers())
			{
				LSampler* nativeSampler = static_cast<LSampler*>(sampler.get());

				assert(nativeSampler->GetSampler() != VK_NULL_HANDLE);

				VkDescriptorImageInfo info{};
				info.sampler = nativeSampler->GetSampler();

				imageInfos.push_back(info);

				VkWriteDescriptorSet writeSampler = {};
				writeSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeSampler.dstSet = m_descriptorSets[currentFrame];
				writeSampler.dstBinding = bindLocation++;
				writeSampler.dstArrayElement = sampleBindLocation;
				writeSampler.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				writeSampler.descriptorCount = 1;
				writeSampler.pImageInfo = &imageInfos.back();
				writes.push_back( writeSampler);
			}

			m_bTextureSamplersDirty = false;
			for (const auto& texture : m_material->GetResource()->GetTextures())
			{
				Texture* tex = static_cast<Texture*>(texture.second->GetTexture()->GetResource());
				uint32_t textureBindLocation = texture.second->GetBindLocation();

				// Imaage View
				imageInfos.push_back({
					.sampler = defaultSampler->GetSampler(),
					.imageView   = tex->GetImageView(),
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				});

				// Check material status of texture type, basically if pink black has been bound but we wern't expecting a texture we switch to a default "normal" texture.
				TextureStatus status = m_material->GetResource()->GetTextureStatus(texture.first);

				if (status == TextureStatus::NotProvided)
				{
					if (m_defaultTextures.contains(texture.first))
					{
						imageInfos.back().imageView = m_defaultTextures[texture.first]->GetImageView();
					}
				}

				VkWriteDescriptorSet writeImage = {};
				writeImage.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeImage.dstSet = m_descriptorSets[currentFrame];
				writeImage.dstBinding = textureBindLocation;
				writeImage.dstArrayElement = 0;
				writeImage.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				writeImage.descriptorCount = 1;
				writeImage.pImageInfo = &imageInfos.back();

				writes.push_back(writeImage);
				bindLocation++;
			}
		}

		for (auto& buffer : m_uniformBuffers)
		{
			buffer->UpdateDescriptors(m_descriptorSets[currentFrame], bindLocation++, true);
			buffer->DumpBuffer();
			//writes.push_back(buffer->GetWrite());
		}

		// Copy anim mats 
		if (m_mesh->GetBoneCount())
		{
			boneBuffer.DataCPUMapped = static_cast<void*>(m_mesh->GetBoneMatrix().get()->data());
			std::memcpy(boneBuffer.DataGPUMapped, boneBuffer.DataCPUMapped, boneBuffer.DataSize);
		}

		std::memcpy(m_vertexDataUniformBuffers[currentFrame].DataGPUMapped, m_vertexDataUniformBuffers[currentFrame].DataCPUMapped, m_vertexDataUniformBuffers[currentFrame].DataSize);
		vkUpdateDescriptorSets(device.GetVkDevice(), writes.size(), writes.data(), 0, nullptr);
	}

	void LRenderBlock::CreateVkDescriptors()
	{
		const LVulkanDevice& device = GraphicsServices::GetContext()->GetVulkanDevice();

		std::shared_ptr<std::vector<glm::mat4>> bones = m_mesh->GetBoneMatrix();
		int numBones = bones != nullptr ? bones->size(): 0;

		// Bone matrices storage buffer
		if (numBones > 0)
		{
			for (auto& boneBuffer : m_boneMatBuffer)
			{
				if (boneBuffer.Buffer == VK_NULL_HANDLE)
				{
					VkBufferCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
					// ensure size is min of 1, dummy mat4 identity when bones is 0 
					createInfo.size = sizeof(glm::mat4) * std::max(1, numBones);
					createInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
					createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				
					if (vkCreateBuffer(device.GetVkDevice(), &createInfo, nullptr, &boneBuffer.Buffer) != VK_SUCCESS) {
						throw std::runtime_error("failed to create bone buffers!");
					}
				}
	
				VkMemoryRequirements memRequirements;
				vkGetBufferMemoryRequirements(device.GetVkDevice(), boneBuffer.Buffer, &memRequirements);
	
				VkMemoryAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
				if (vkAllocateMemory(device.GetVkDevice(), &allocInfo, nullptr, &boneBuffer.VKDeviceMemory) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate bone buffer memory!");
				}
	
				vkBindBufferMemory(device.GetVkDevice(), boneBuffer.Buffer, boneBuffer.VKDeviceMemory, 0);
				boneBuffer.DataSize = sizeof(glm::mat4) * std::max(1, numBones);
				vkMapMemory(device.GetVkDevice(), boneBuffer.VKDeviceMemory, 0, boneBuffer.DataSize, 0, &boneBuffer.DataGPUMapped);
				boneBuffer.DataCPUMapped = static_cast<void*>(m_mesh->GetBoneMatrix().get()->data());
				std::memcpy(boneBuffer.DataGPUMapped, boneBuffer.DataCPUMapped, boneBuffer.DataSize);
			}
		}

		// Allocate Unifrom buffer memory 
		for (auto& uniformBuffer : m_vertexDataUniformBuffers)
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
			allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			if (vkAllocateMemory(device.GetVkDevice(), &allocInfo, nullptr, &uniformBuffer.VKDeviceMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate vertex buffer memory!");
			}

			vkBindBufferMemory(device.GetVkDevice(), uniformBuffer.Buffer, uniformBuffer.VKDeviceMemory, 0);
			uniformBuffer.DataSize = sizeof(VertexData);
			vkMapMemory(device.GetVkDevice(), uniformBuffer.VKDeviceMemory, 0, uniformBuffer.DataSize, 0, &uniformBuffer.DataGPUMapped);
			uniformBuffer.DataCPUMapped = &m_vertexData;
			std::memcpy(uniformBuffer.DataGPUMapped, uniformBuffer.DataCPUMapped, uniformBuffer.DataSize);
		}

		int bindLocation = 0;
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = bindLocation++;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1; 
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;   
		bindings.push_back(uboLayoutBinding);

		if (m_mesh->GetBoneCount())
		{
			VkDescriptorSetLayoutBinding boneLayoutBinding{};
			boneLayoutBinding.binding = bindLocation++;
			boneLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			boneLayoutBinding.descriptorCount = 1;
			boneLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			boneLayoutBinding.pImmutableSamplers = nullptr;
			bindings.push_back(boneLayoutBinding);
		}

		// Todo add support for multiple samplers
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = bindLocation++; 
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindings.push_back(samplerLayoutBinding);

		for (const auto& texture : m_material->GetResource()->GetTextures())
		{
			Texture* tex = static_cast<Texture*>(texture.second->GetTexture()->GetResource());
			uint32_t texturebindLocation = texture.second->GetBindLocation();

			VkDescriptorSetLayoutBinding imageLayoutBinding{};
			imageLayoutBinding.binding = texturebindLocation;
			imageLayoutBinding.descriptorCount = 1;
			imageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			imageLayoutBinding.pImmutableSamplers = nullptr;
			imageLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			bindings.push_back(imageLayoutBinding);
			bindLocation++;
		}

		for (const auto& buffer : m_uniformBuffers) 
		{
			VkDescriptorSetLayoutBinding storagebinding{};
			storagebinding.binding = bindLocation++;
			storagebinding.descriptorType = buffer->GetDescriptorType();
			storagebinding.descriptorCount = 1;
			storagebinding.stageFlags = buffer->GetShaderStage();
			storagebinding.pImmutableSamplers = nullptr;
			bindings.push_back(storagebinding);
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
            },
			{
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 100
            }
        };

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 4;
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

		m_vkPipelineLayoutCreateInfo = {};
		m_vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		m_vkPipelineLayoutCreateInfo.setLayoutCount = 1; 
		m_vkPipelineLayoutCreateInfo.pSetLayouts = &m_vkDescriptorSetLayout;
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

		std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;

		for (int i = 0; i < activeTarget->GetAttachmentCount(); ++i)
		{
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

			blendAttachments.push_back(colorBlendAttachment);
		}
		
		VkPipelineColorBlendStateCreateInfo colorBlending = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.attachmentCount = activeTarget->GetAttachmentCount(),
			.pAttachments = blendAttachments.data()
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
