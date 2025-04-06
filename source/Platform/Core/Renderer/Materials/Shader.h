#pragma once

#include <Platform/Core/Renderer/Materials/AShader.h>
#include <Resources/AResource.h>

namespace Lemonade
{
	class Shader : AShader
	{
	protected:
		virtual bool LoadResource(std::string path) override;
	private:
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType);
		const std::string m_meta = ".meta";
	};
}