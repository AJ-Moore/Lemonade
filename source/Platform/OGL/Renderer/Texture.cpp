
#include <GL/gl.h>
#include <LCommon.h>

#ifdef RENDERER_OPENGL
#include <Platform/OGL/Renderer/Texture.h>
#include <Platform/Core/Renderer/Materials/TextureType.h>

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

	void Texture::LoadNativeTextureFromSurface(SDL_Surface* surface)
	{
		if (m_textureId > 0)
		{
			glDeleteTextures(1, &m_textureId);
		}
		
		glBindTexture(GL_TEXTURE_2D, m_textureId);
		GLint internalFormat = TextureData::GetNativeTextureFormat(GetTextureFormat());

		glTexImage2D(GL_TEXTURE_2D,
			0,//Mipmap Level
			internalFormat, //bit per pixel
			surface->w,
			surface->h,
			0,//texture border 	
			internalFormat,
			GL_UNSIGNED_BYTE,
			surface->pixels);
	}

	void Texture::LoadNativeTextureFromPixels(const std::vector<Colour>& data)
	{
		if (m_textureId > 0)
		{
			glDeleteTextures(1, &m_textureId);
		}

		// create the surface from our pixel data!
		GLvoid* pixelDat = (GLvoid*)(&data.front());
		int dimensions = (int)sqrt(data.size());

		glBindTexture(GL_TEXTURE_2D, m_textureId);

		glTexImage2D(GL_TEXTURE_2D,
			0,//Mipmap Level
			GL_RGBA, //bit per pixel
			dimensions,
			dimensions,
			0,//texture border 	
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pixelDat);

		m_width = dimensions;
		m_height = dimensions;
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
