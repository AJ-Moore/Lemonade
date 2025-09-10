#pragma once
#include <Platform/Vulkan/Renderer/LGLSLShader.h>
#include <LCommon.h>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

#ifdef RENDERER_VULKAN

namespace Lemonade
{
	class LEMONADE_API LShader : public LGLSLShader 
	{
		friend class Material;
		friend class LRenderBlock;
	public:
		const VkPipelineShaderStageCreateInfo& GetShaderPipelineInfo() const { return m_pipelineShaderStageCreateInfo; }
		VkShaderStageFlagBits GetShaderStage(ShaderType type);
	protected:
		virtual void OnShaderCompiled(std::vector<uint32_t>& code) override;
	private:
		VkShaderModule m_shaderModule;
		VkPipelineShaderStageCreateInfo m_pipelineShaderStageCreateInfo;
		static const std::unordered_map<ShaderType, VkShaderStageFlagBits> m_shaderStageLookup;
	};
}
#endif