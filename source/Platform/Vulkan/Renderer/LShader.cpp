

#ifdef RENDERER_VULKAN
#include <Renderer/Platform/Vulkan/UShader_Vulkan.h>
#include <iostream>

namespace UpsilonEngine
{
	GLint UShader::m_currentProgram = -1;

	void UShader::load()
	{
	}

	uint32 UShader::loadAndCompileShader(const std::string& shaderFile, UShaderType shaderType)
	{
		return 0;
	}

	void UShader::bindUniformBuffer(const std::string& name, uint32 buffer) const
	{
	}

	void UShader::bindTextureBufferObject(const std::string& name, uint32 textureUnit) const
	{
	}

	void UShader::bindShaderStorageBuffer(const std::string& name, uint32 buffer) const
	{
	}

	GLint UShader::getUniformLocation(const std::string& name) const
	{
		return -1;
	}

	void UShader::setUniformSampler2DTextureUnit(const std::string& Sampler2DName, GLint textureUnit) const
	{
	}

	void UShader::setUniformSampler2DTextureUnit(const GLint& location, GLint textureUnit) const
	{
	}

	void UShader::setUniformVec3v(const std::string& name, const std::vector<glm::vec3>& data) const
	{
	}

	void UShader::uniformMat4fv(const GLint location, uint32 Size, bool Transpose, const glm::mat4& Matrix) const
	{
	}

	void UShader::uniformMat4fv(const std::string& Name, uint32 Size, bool Transpose, const glm::mat4& Matrix) const
	{
	}

	void UShader::setUniformVec4(const std::string& name, glm::vec4 input) const
	{
	}

	void UShader::setUniformBool(const std::string& name, bool input) const
	{
	}

	void UShader::setUniformBool(const GLint& location, bool input) const
	{
	}

	void UShader::setUniformVec3(const std::string& name, glm::vec3 input) const
	{
	}

	void UShader::setUniformVec3(const GLint& location, const glm::vec3& input) const
	{
	}

	void UShader::setUniformiVec2(const std::string& name, glm::ivec2 input) const
	{
	}

	void UShader::setUniform1i(const std::string& name, int value) const
	{
	}

	void UShader::setUniformfv(const std::string& name, const std::vector<float>& arrr) const
	{
	}

	void UShader::setUniformfv(const std::string& name, float value) const
	{
	}

	void UShader::bindProgram() const
	{
	}

	void UShader::setUniformHandlei64(const std::string& name, const std::vector<GLuint64>& data)
	{
	}
}

#endif
