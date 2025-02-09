#pragma once

#include <Common.h>
#include <Core/GameObject.h>
#include <Renderer/Shader.h>

namespace CraftyBlocks
{
	class AShader : public GameObject
	{
	public:
		virtual void Bind() = 0;
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType) = 0;
		virtual void SetUniformMat4fv(const std::string& Name, uint32 Size, bool Transpose, const glm::mat4& Matrix) const;
		virtual void SetUniformMat4fv(int32 location, uint32 Size, bool Transpose, const glm::mat4& Matrix) const;
		virtual void SetUniformVec3v(const std::string& name, const std::vector<glm::vec3>& data) const;
		virtual void SetUniformVec4(const std::string& name, glm::vec4 input) const;
		virtual void SetUniformBool(const std::string& name, bool input) const;
		virtual void SetUniformBool(int32 location, bool input) const;
		virtual void SetUniformVec3(int32 location, const glm::vec3& input) const;
		virtual void SetUniformVec3(const std::string& name, glm::vec3 input) const;
		virtual void SetUniformiVec2(const std::string& name, glm::ivec2 input) const;
		virtual void SetUniformfv(const std::string& name, const std::vector<float>& arrr) const;
		virtual void SetUniformfv(const std::string& name, float value) const;
		virtual void SetUniform1i(const std::string& name, int value) const;
		virtual void SetUniformHandlei64(const std::string& name, const std::vector<GLuint64>& data);
		virtual void BindUniformBuffer(const std::string& name, uint32 buffer) const;
		virtual void BindTextureBufferObject(const std::string& name, uint32 textureUnit) const;

		virtual void SetUniformSampler2DTextureUnit(const std::string& Sampler2DName, int32 Location) const;

		const std::string VertexPositionAtrib = "position";
		const std::string UVSAtrib = "texCoord";
		const std::string ColourAtrib = "colour";
		const std::string NormalAtrib = "normal";
		const std::string TangentAtrib = "tangent";
		const std::string BiTangentAtrib = "biTangent";
		const std::string WeightsAtrib = "boneWeights";
		const std::string BoneIdAtrib = "boneIds";
		const std::string InstanceMatAtrib = "instanceMat";

		static const std::unordered_map<std::string, ShaderType> m_shaderTypeLookup;
		static ShaderType GetShaderType(std::string type);
	};
}