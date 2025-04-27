#include <Platform/Core/Renderer/Materials/AShaderProgram.h>

namespace Lemonade
{
    void AShaderProgram::Bind()
    {
    }

    uint32 AShaderProgram::LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType)
    {
        return uint32();
    }

    void AShaderProgram::BindShaderStorageBuffer(const std::string& name, uint32 buffer) const
    {
    }
}