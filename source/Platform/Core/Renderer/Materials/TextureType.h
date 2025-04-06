#pragma once

#include <Platform/Core/Renderer/Materials/TextureUtils.h>

namespace Lemonade
{
	enum class TextureType
	{
		None,
		Diffuse,
		Specular,
		Ambient,
		Emissive,
		Height,
		Normal,
		Shininess,
		Opacity,
		Displacement,
		Lightmap,
		Reflection,
		BaseColour,
		NormalCamera,
		EmissionColour,
		Metalness,
		Roughness,
		AmbientOcclusion,
		Unknown
	};

	enum class TextureBindType {
		Texture2D,
		TextureArray2D,
		CubeMap,
		Colour,
		None
	};

	class TextureData
	{
	public:
		TextureData() = delete;
		TextureData(TextureType type);
		TextureData(TextureType type, std::string path);
		TextureData(TextureType type, std::string path, std::string shaderVar);
		std::string GetName();
		std::string GetPath() const { return m_path; }
		std::string GetShaderVariable() const { return m_shaderVariable; }
		TextureType GetTextureType() const { return m_textureType; }
		static std::string GetStringTypeForBindType(TextureBindType bindType);
		static TextureBindType GetTextureBindType(std::string);

		// Implemented in native platform Texture.cpp
		static int GetNativeTextureFormat(const TextureFormat& format);

	private:
		static const std::unordered_map<std::string, TextureType> m_textureLookup;
		static const std::unordered_map<std::string, TextureBindType> m_textureBindLookup;
		static const std::unordered_map<std::string, TextureClamp> m_textureClampLookup;
		static const std::unordered_map<std::string, TextureFilter> m_textureFilterLookup;
		static const std::unordered_map<TextureFormat, int> m_nativeTextureFormatLookup;
		std::string m_shaderVariable;
		std::string m_path;
		TextureType m_textureType;
	};
}