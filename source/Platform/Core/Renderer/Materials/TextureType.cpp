#include "Util/Logger.h"
#include <unordered_map>
#include <Platform/Core/Renderer/Materials/TextureType.h>
#include <LCommon.h>

namespace Lemonade
{
	using CitrusCore::Logger;

	const std::unordered_map<std::string, TextureType> TextureData::m_textureLookup =
	{
		{"None", TextureType::None},
		{"Diffuse", TextureType::Diffuse},
		{"texture", TextureType::Diffuse},
		{"Specular", TextureType::Specular},
		{"Ambient", TextureType::Ambient},
		{"Emissive", TextureType::Emissive},
		{"Height", TextureType::Height},
		{"Normal", TextureType::Normal},
		{"Shininess", TextureType::Shininess},
		{"Opacity", TextureType::Opacity},
		{"Displacement", TextureType::Displacement},
		{"Lightmap", TextureType::Lightmap},
		{"Reflection", TextureType::Reflection},

		{"BaseColour", TextureType::BaseColour},
		{"NormalCamera", TextureType::NormalCamera},
		{"EmissionColour", TextureType::EmissionColour},
		{"Metalness", TextureType::Metalness},
		{"Roughness", TextureType::Roughness},
		{"AmbientOcclusion", TextureType::AmbientOcclusion}
	};

	const std::unordered_map<std::string, TextureBindType> TextureData::m_textureBindLookup =
	{
		{"none", TextureBindType::None},
		{"texture_2d", TextureBindType::Texture2D},
		{"cube_map", TextureBindType::CubeMap},
		{"texture_array_2d", TextureBindType::TextureArray2D},
		{"colour", TextureBindType::Colour},
	};

	const std::unordered_map<std::string, TextureClamp> TextureData::m_textureClampLookup = {
		{"none", TextureClamp::None},
		{"clamp", TextureClamp::Clamp},
		{"repeat", TextureClamp::Repeat}
	};

	const std::unordered_map<std::string, TextureFilter> TextureData::m_textureFilterLookup = {
		{"none", TextureFilter::None},
		{"linear", TextureFilter::Linear},
		{"nearest", TextureFilter::NearestNeighbour}
	};

	TextureData::TextureData(TextureType type, CitrusCore::ResourcePtr<ATexture> texture)
	{
		m_textureType = type;
		m_texture = texture;
	}

	TextureType TextureData::GetTextureType(std::string type)
	{
		auto result = m_textureLookup.find(type);

		if (result != m_textureLookup.end())
		{
			return result->second;
		}

		return TextureType::None;
	}

	std::string TextureData::GetName()
	{
		for (auto texType : m_textureLookup)
		{
			if (texType.second == m_textureType)
			{
				return texType.first;
			}
		}

		return "Unknown";
	}

	std::string TextureData::GetStringTypeForBindType(TextureBindType bindType)
	{
		for (const auto& [key, value] : m_textureBindLookup)
		{
			if (value == bindType)
			{
				return key;
			}
		}
		
		Logger::Log(Logger::ERROR, "Key not found, missing type in bind lookup!");

		// Might seem unintuitive but in most cases it's safer to fallback to UTexture_2D
		return "UTexture_2D";
	}

	TextureBindType TextureData::GetTextureBindType(std::string type)
	{
		auto iterator = m_textureBindLookup.find(type);

		if (iterator != m_textureBindLookup.end())
		{
			return iterator->second;
		}

		return TextureBindType::Texture2D;
	}
}