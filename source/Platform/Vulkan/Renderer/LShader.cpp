#include <Platform/Core/Renderer/Materials/AShader.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LShader.h>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

#ifdef RENDERER_VULKAN

namespace Lemonade
{
        const std::unordered_map<ShaderType, VkShaderStageFlagBits> LShader::m_shaderStageLookup =
        {
                { ShaderType::Vertex,VK_SHADER_STAGE_VERTEX_BIT },
                { ShaderType::Fragment, VK_SHADER_STAGE_FRAGMENT_BIT },
                { ShaderType::Geometry, VK_SHADER_STAGE_GEOMETRY_BIT },
                { ShaderType::TessellationControl, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT },
                { ShaderType::TessellationEvaluation, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT },
                { ShaderType::Compute, VK_SHADER_STAGE_COMPUTE_BIT },
                { ShaderType::Mesh, VK_SHADER_STAGE_MESH_BIT_NV },
                { ShaderType::Task, VK_SHADER_STAGE_TASK_BIT_NV },
                { ShaderType::RayGeneration, VK_SHADER_STAGE_RAYGEN_BIT_KHR },
                { ShaderType::AnyHit, VK_SHADER_STAGE_ANY_HIT_BIT_KHR },
                { ShaderType::ClosestHit, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },
                { ShaderType::Miss, VK_SHADER_STAGE_MISS_BIT_KHR },
                { ShaderType::Intersection, VK_SHADER_STAGE_INTERSECTION_BIT_KHR },
        };

        VkShaderStageFlagBits LShader::GetShaderStage(ShaderType type)
	{
		auto result = m_shaderStageLookup.find(type);

		if (result != m_shaderStageLookup.end())
		{
			return result->second;
		}

                throw("Shouldn't happen?");
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

        void LShader::OnShaderCompiled(std::vector<uint32_t>& code)
        {
                /// VULKAN SPECIFIC BIT, REST CAN BE MADE COMMON! 
		// Create a Vulkan shader module from the compilation result
		VkShaderModuleCreateInfo shaderModuleCI{};
		shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCI.codeSize = code.size() * sizeof(uint32_t);
		shaderModuleCI.pCode = code.data();
		vkCreateShaderModule(GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice(), &shaderModuleCI, nullptr, &m_shaderModule);

                m_pipelineShaderStageCreateInfo = {};
                m_pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_pipelineShaderStageCreateInfo.pNext = nullptr; 
                m_pipelineShaderStageCreateInfo.flags = 0;
                m_pipelineShaderStageCreateInfo.stage = GetShaderStage(GetShaderType());
                m_pipelineShaderStageCreateInfo.pName = "main";
                m_pipelineShaderStageCreateInfo.module = m_shaderModule;

        }
}

#endif
