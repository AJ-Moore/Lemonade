
#include "Platform/Core/Renderer/Materials/AShader.h"
#include <Platform/Core/Renderer/Materials/AShaderProgram.h>
#include <Platform/Vulkan/Renderer/LShader.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Vulkan/Renderer/LShaderProgram.h>
#include <vulkan/vulkan_core.h>

namespace Lemonade {

    void LShaderProgram::AddShader(CitrusCore::ResourcePtr<AShader> shader)
    {
        AShaderProgram::AddShader(shader);
        UpdateVkPipelineShaders();
    }

    void LShaderProgram::UpdateVkPipelineShaders() 
    {
        m_shaderCreateInfos.clear();

        for (const CitrusCore::ResourcePtr<AShader> shader : GetShaders())
        {
            LShader* native = (LShader*)shader->GetResource();

            if (shader->GetResource()->GetShaderType() != ShaderType::Compute)
            {
                m_shaderCreateInfos.push_back(native->GetShaderPipelineInfo());
            }
            else {
                m_shaderComputeCreateInfos.push_back(native->GetShaderPipelineInfo());
            }
        }
    }

    VkPipelineShaderStageCreateInfo* LShaderProgram::GetComputeShaderInfo()
    {
        for (auto& createInfo : m_shaderComputeCreateInfos)
        {
            if (createInfo.stage == VK_SHADER_STAGE_COMPUTE_BIT)
            {
                return &createInfo;
            }
        }

        return nullptr;
    }
}