
#include <LCommon.h>

#ifdef RENDERER_OPENGL
#include <platform/ogl/Renderer/Texture.h>
#include <Core/Renderer/Materials/TextureType.h>

namespace Lemonade
{
	// Native Texture Format Lookup 
	const std::unordered_map<TextureFormat, int> TextureData::m_nativeTextureFormatLookup = {
		{TextureFormat::LEMONADE_RGBA8888, GL_RGBA },
		{TextureFormat::LEMONADE_BGRA8888, GL_BGRA },
		{TextureFormat::LEMONADE_RGB888, GL_RGB },
		{TextureFormat::LEMONADE_BGR888, GL_BGR },
		{TextureFormat::LEMONADE_UNKNOWN, -1 },
	};

	int TextureData::GetNativeTextureFormat(const TextureFormat& format)
	{
		std::unordered_map<TextureFormat, int>::const_iterator iter = m_nativeTextureFormatLookup.find(format);
		
		if (iter != m_nativeTextureFormatLookup.end())
		{
			return iter->second;
		}

		return -1;
	}

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_textureId);
	}

	void Texture::Bind(uint textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_textureId);
	}
}

#endif 
