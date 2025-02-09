#include "AShader.h"
#include <Common.h>

namespace CraftyBlocks
{
	const std::unordered_map<std::string, ShaderType> AShader::m_shaderTypeLookup =
	{
		{"None", ShaderType::None},
		{"Fragment", ShaderType::Fragment},
		{"Vertex", ShaderType::Vertex},
		{"Geometry", ShaderType::Geometry},
	};

	ShaderType AShader::GetShaderType(std::string type)
	{
		std::string ltype = StringUtil::ToLower(type);
		auto iterator = m_shaderTypeLookup.find(ltype);

		if (iterator != m_shaderTypeLookup.end())
		{
			return iterator->second;
		}

		return ShaderType::None;
	}

	void AShader::SetUniformMat4fv(const std::string& Name, uint32 Size, bool Transpose, const glm::mat4& Matrix) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformMat4fv(int32 location, uint32 Size, bool Transpose, const glm::mat4& Matrix) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformVec3v(const std::string& name, const std::vector<glm::vec3>& data) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformVec4(const std::string& name, glm::vec4 input) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformBool(const std::string& name, bool input) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformBool(int32 location, bool input) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformVec3(int32 location, const glm::vec3& input) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformVec3(const std::string& name, glm::vec3 input) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformiVec2(const std::string& name, glm::ivec2 input) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformfv(const std::string& name, const std::vector<float>& arrr) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformfv(const std::string& name, float value) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniform1i(const std::string& name, int value) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformHandlei64(const std::string& name, const std::vector<GLuint64>& data)
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::BindUniformBuffer(const std::string& name, uint32 buffer) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::BindTextureBufferObject(const std::string& name, uint32 textureUnit) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
	void AShader::SetUniformSampler2DTextureUnit(const std::string& Sampler2DName, int32 Location) const
	{
		Log(Logger::WARN, "Function not implemented [%s]", __FUNCTION__);
	}
}
