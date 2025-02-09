#pragma once

#include <Core/ResourceManager/Resource.h>

namespace CraftyBlocks
{
	enum class ShaderType
	{
		None,
		Fragment,
		Vertex,
		Geometry
	};

	class Shader : Resource
	{
	protected:
		virtual bool LoadResource(std::string path) override;
	private:
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType);
		const std::string m_meta = ".meta";
	};
}