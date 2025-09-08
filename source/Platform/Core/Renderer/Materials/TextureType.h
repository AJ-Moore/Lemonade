#pragma once

#include "LCommon.h"
// #include "Platform/Core/Renderer/Materials/ATexture.h"
// //#include <Platform/Core/Renderer/Materials/ATexture.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Renderer/Materials/TextureUtils.h>

namespace Lemonade
{
	class ATexture;
	// Todo explicitly map to assimps texture types
	enum class LEMONADE_API TextureType
	{
		None = 0x0,
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

	enum class LEMONADE_API TextureBindType {
		Texture2D,
		TextureArray2D,
		CubeMap,
		Colour,
		None
	};

	class LEMONADE_API TextureData
	{
	public:
		TextureData() = delete;
		TextureData(TextureType type, CitrusCore::ResourcePtr<ATexture>, int bindLocation);
		std::string GetName();
		std::string GetPath() const { return m_path; }
		std::string GetShaderVariable() const { return m_shaderVariable; }
		TextureType GetTextureType() const { return m_textureType; }
		int GetBindLocation() const { return m_bindLocation; }
		static std::string GetStringTypeForBindType(TextureBindType bindType);
		static TextureBindType GetTextureBindType(std::string);
		static TextureType GetTextureType(std::string type);

		// Implemented in native platform Texture.cpp
		static int GetNativeTextureFormat(const TextureFormat& format);

		CitrusCore::ResourcePtr<ATexture> GetTexture() const noexcept { return m_texture; }

	private:
		static const std::unordered_map<std::string, TextureType> m_textureLookup;
		static const std::unordered_map<std::string, TextureBindType> m_textureBindLookup;
		static const std::unordered_map<std::string, TextureClamp> m_textureClampLookup;
		static const std::unordered_map<std::string, TextureFilter> m_textureFilterLookup;
		static const std::unordered_map<TextureFormat, int> m_nativeTextureFormatLookup;
		std::string m_shaderVariable;
		std::string m_path;
		TextureType m_textureType;
		int m_bindLocation;

		CitrusCore::ResourcePtr<ATexture> m_texture;
	};
}