#pragma once
#include "Platform/Core/Renderer/Materials/TextureType.h"
#include "Platform/Vulkan/Materials/Texture.h"
#include <Platform/Core/Renderer/Geometry/Mesh.h>
#include <Platform/Core/Renderer/Geometry/PrimitiveMode.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <Spatial/Transform.h>
#include <LCommon.h>
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <Platform/Core/Renderer/Materials/VertexData.h>

#ifdef RENDERER_VULKAN
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>

namespace Lemonade
{
	struct LEMONADE_API LVKBuffer
	{
		VkBuffer Buffer;
		void* DataCPUMapped = nullptr;
		void* DataGPUMapped = nullptr;
		size_t DataSize = 0;
		VkDeviceMemory VKDeviceMemory;
		size_t Stride;
		int Binding = 0;
		VkFormat Format = VK_FORMAT_R32G32B32_SFLOAT;
	};

	/** URender Block Class - Used to store render state information */
	class LEMONADE_API LRenderBlock : public ARenderBlock
	{
		enum class VKBufferType 
		{
			Position,
			Normal, 
			FaceNormal,
			Colour,
			UV,
			UV3D,
			Tangents, 
			BiTangents, 
			BoneWeights, 
			BoneIds,
		};

		friend class LRenderLayer;
		friend class LRenderer;
	public:
		LRenderBlock();
		virtual ~LRenderBlock() {}

		/// Sets the draw mode used by this block.
		virtual void SetDrawMode(PrimitiveMode mode);

		PrimitiveMode GetDrawMode() const { return (PrimitiveMode)m_primitiveMode; }

		LVKBuffer GetLVKBuffer(uint32_t currentFrame) { return m_uniformBuffers.at(currentFrame); }

	protected:
		/// Called to perform initialisation 
		virtual bool Init();

		/// Called prior to object deletion to perform any unload actions
		virtual void Unload();

		/// Called to perform buffer updates
		virtual void Update();

		/// Called to render the block 
		virtual void Render();

		/// Dumbs the vertex data to the buffer
		virtual void DumpBufferData();

		/// Dmps the animation buffer data 
		virtual void DumpAnimationData();

		// Set/ configure vertex attributes.
		virtual void SetVertexAttributes();

		std::vector<VkBuffer> m_vkBuffers;
		std::vector<VkDeviceSize> m_vkOffsets;

		std::unordered_map<VKBufferType, LVKBuffer> m_vertexBuffers = {
			{VKBufferType::Position, {} },
			{VKBufferType::Normal, {} },
			{VKBufferType::FaceNormal, {} },
			{VKBufferType::Colour, {} },
			{VKBufferType::UV, {} },
			{VKBufferType::UV3D, {} },
			{VKBufferType::Tangents,  {} },
			{VKBufferType::BiTangents,  {} },
			{VKBufferType::BoneWeights,  {} },
			{VKBufferType::BoneIds,  {} }
		};

		const std::vector<VKBufferType> m_bufferOrder = {
			VKBufferType::Position,
			VKBufferType::Normal,
			VKBufferType::FaceNormal,
			VKBufferType::Colour,
			VKBufferType::UV,
			VKBufferType::UV3D,
			VKBufferType::Tangents,
			VKBufferType::BiTangents,
			VKBufferType::BoneWeights,
			VKBufferType::BoneIds
		};

	private: 

		void SetUniforms();
		void CreateVkPipeline();
		void CreateVkDescriptors();
		std::vector<LVKBuffer> m_uniformBuffers;
		std::vector<VkDescriptorSet> m_descriptorSets;

		// Vertex Input
		std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;

		// VK Pipeline
		VkPipeline m_vkPipeline = VK_NULL_HANDLE;

		// Shader Stage
		VertexData m_vertexData;
		VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_vkDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool m_vkDescriptorPool = VK_NULL_HANDLE;

		VkPipelineLayoutCreateInfo m_vkPipelineLayoutCreateInfo;
		VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
		VkPipelineRasterizationStateCreateInfo m_rasterizer;
		VkPipelineColorBlendAttachmentState m_colorBlendAttachment; 
		VkPipelineMultisampleStateCreateInfo m_multisampling;
		VkPipelineVertexInputStateCreateInfo m_vertexInputInfo;

		Texture m_defaultDiffuse;
		Texture m_defaultRoughness;
		Texture m_defaultNormal;
		Texture m_defaultAo;
		Texture m_defaultEmission;
		Texture m_defaultMetalness;

		std::unordered_map<TextureType, Texture*> m_defaultTextures;
	};
}
#endif
