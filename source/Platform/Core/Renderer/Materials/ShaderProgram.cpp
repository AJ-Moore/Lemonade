#include "ShaderProgram.h"

namespace CraftyBlocks
{
    void ShaderProgram::Bind()
    {
    }

    uint32 ShaderProgram::LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType)
    {
        return uint32();
    }

    void ShaderProgram::SetUniformMat4fv(const std::string& Name, uint32 Size, bool Transpose, const glm::mat4& Matrix) const
    {
    }

    void ShaderProgram::SetUniformMat4fv(int32 location, uint32 Size, bool Transpose, const glm::mat4& Matrix) const
    {
    }

    void ShaderProgram::SetUniformVec3v(const std::string& name, const std::vector<glm::vec3>& data) const
    {
    }

    void ShaderProgram::SetUniformVec4(const std::string& name, glm::vec4 input) const
    {
    }

    void ShaderProgram::SetUniformBool(const std::string& name, bool input) const
    {
    }

    void ShaderProgram::SetUniformBool(int32 location, bool input) const
    {
    }

    void ShaderProgram::SetUniformVec3(int32 location, const glm::vec3& input) const
    {
    }

    void ShaderProgram::SetUniformVec3(const std::string& name, glm::vec3 input) const
    {
    }

    void ShaderProgram::SetUniformiVec2(const std::string& name, glm::ivec2 input) const
    {
    }

    void ShaderProgram::SetUniformfv(const std::string& name, const std::vector<float>& arrr) const
    {
    }

    void ShaderProgram::SetUniformfv(const std::string& name, float value) const
    {
    }

    void ShaderProgram::SetUniform1i(const std::string& name, int value) const
    {
    }

    void ShaderProgram::SetUniformHandlei64(const std::string& name, const std::vector<GLuint64>& data)
    {
    }

    void ShaderProgram::BindUniformBuffer(const std::string& name, uint32 buffer) const
    {
    }

    void ShaderProgram::BindTextureBufferObject(const std::string& name, uint32 textureUnit) const
    {
    }

    void ShaderProgram::SetUniformSampler2DTextureUnit(const std::string& Sampler2DName, int32 Location) const
    {
    }

    void ShaderProgram::BindShaderStorageBuffer(const std::string& name, uint32 buffer) const
    {
    }
}