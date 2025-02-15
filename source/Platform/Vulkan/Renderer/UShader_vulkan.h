#pragma once

#include <UCommon.h>
#ifdef RENDERER_VULKAN
#include <Renderer/AShader.h>
#include <unordered_map>

namespace UpsilonEngine
{
	class DLLExport UShader : AShader
	{
		friend class UMaterial;
		friend class URenderBlock;
	public:
		virtual void load();
		virtual uint32 loadAndCompileShader(const std::string& shaderFile, UShaderType shaderType);
		virtual void setUniformVec3v(const std::string& name, const std::vector<glm::vec3>& data) const;
		virtual void uniformMat4fv(const std::string& Name, uint32 Size, bool Transpose, const glm::mat4& Matrix) const;
		virtual void uniformMat4fv(const GLint location, uint32 Size, bool Transpose, const glm::mat4& Matrix) const;
		virtual void setUniformVec4(const std::string& name, glm::vec4 input) const;
		virtual void setUniformBool(const std::string& name, bool input) const;
		virtual void setUniformBool(const GLint& location, bool input) const;
		virtual void setUniformVec3(const GLint& location, const glm::vec3& input) const;
		virtual void setUniformVec3(const std::string& name, glm::vec3 input) const;
		virtual void setUniformiVec2(const std::string& name, glm::ivec2 input) const;
		virtual void setUniformfv(const std::string& name, const std::vector<float>& arrr) const;
		virtual void setUniformfv(const std::string& name, float value) const;
		virtual void setUniform1i(const std::string& name, int value) const;
		virtual void setUniformHandlei64(const std::string& name, const std::vector<GLuint64>& data);
		virtual void bindUniformBuffer(const std::string& name, uint32 buffer) const;
		virtual void bindTextureBufferObject(const std::string& name, uint32 textureUnit) const;
		virtual void bindShaderStorageBuffer(const std::string& name, uint32 buffer) const;
		virtual void bindProgram() const;

		virtual void setUniformSampler2DTextureUnit(const std::string& Sampler2DName, GLint Location) const;
		virtual void setUniformSampler2DTextureUnit(const GLint& location, GLint textureUnit) const;
		virtual GLint getUniformLocation(const std::string& name) const;
	protected:
		virtual void setWVPMatrix(const glm::mat4&) const {}
		virtual void setModelMatrix(const glm::mat4&) const {}
		virtual void setViewMatrix(const glm::mat4&) const {}
		virtual void setProjectionMatrix(const glm::mat4&) const {}
		virtual void setCameraPosition(const glm::vec3&) const {}
		virtual void setCameraLook(const glm::vec3&) const {}

		mutable std::unordered_map<String, GLuint> m_uniformLocations;
		static GLint m_currentProgram;
	};
}
#endif