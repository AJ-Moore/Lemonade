#include <Util/StringUtil.h>
#include <Util/Logger.h>
#include <Platform/Core/Renderer/Materials/AShader.h>
#include <LCommon.h>

namespace Lemonade
{
	using CitrusCore::Logger;
	using CitrusCore::StringUtil;

	const std::unordered_map<std::string, ShaderType> AShader::m_shaderTypeLookup =
	{
		{"none", ShaderType::None},
		{"fragment", ShaderType::Fragment},
		{"vertex", ShaderType::Vertex},
		{"geometry", ShaderType::Geometry},
	};

	ShaderType AShader::GetShaderTypeForString(std::string type)
	{
		std::string ltype = StringUtil::ToLower(type);
		auto iterator = m_shaderTypeLookup.find(ltype);

		if (iterator != m_shaderTypeLookup.end())
		{
			return iterator->second;
		}

		return ShaderType::None;
	}

	void AShader::SetShaderType(ShaderType type)
	{
		m_shaderType = type;
	}
}
