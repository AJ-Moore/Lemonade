
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

	int Texture::GetNativeTextureFormat(TextureFormat format);
	{
		if (surface == nullptr)
		{
			Citrus::Logger::Log(Citrus::Logger::ERROR, "Unable to get texture format.");
			return GL_RGBA;
		}

        switch (surface->format) {
            case SDL_PIXELFORMAT_RGB24:
                return GL_RGB;
            case SDL_PIXELFORMAT_RGBA8888:
                return GL_RGBA;
            case SDL_PIXELFORMAT_ABGR8888:
                return GL_ABGR_EXT;
            case SDL_PIXELFORMAT_BGR24:
                return GL_BGR;
            case SDL_PIXELFORMAT_BGRA8888:
                return GL_BGRA;
            case SDL_PIXELFORMAT_RGB565:
                return GL_RGB; 
        default:
            Citrus::Logger::Log(Citrus::Logger::ERROR, "Texture unsupported format: %s", SDL_GetPixelFormatName(surface->format));
            return -1;
        }
	}
}

#endif 
