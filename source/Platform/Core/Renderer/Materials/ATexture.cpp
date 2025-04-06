#include "ATexture.h"
#include <Util/Logger.h>
#include <filesystem>
#include <Platform/Core/Renderer/Materials/TextureType.h>

namespace Lemonade
{
	using CitrusCore::Logger;

	TextureFormat ATexture::GetTextureFormat(SDL_Surface* surface) const
	{
		if (surface == nullptr)
		{
			Logger::Log(Logger::ERROR, "Unable to get texture format.");
			return TextureFormat::LEMONADE_UNKNOWN;
		}

		switch (surface->format) {
		case SDL_PIXELFORMAT_RGB24:
			return TextureFormat::LEMONADE_RGB888;
		case SDL_PIXELFORMAT_RGBA8888:
			return TextureFormat::LEMONADE_RGBA8888;
		case SDL_PIXELFORMAT_BGRA8888:
			return TextureFormat::LEMONADE_BGRA8888;
		case SDL_PIXELFORMAT_BGR24:
			return TextureFormat::LEMONADE_BGR888;
		default:
			Logger::Log(Logger::ERROR, "Texture unsupported format: %s", SDL_GetPixelFormatName(surface->format));
			return TextureFormat::LEMONADE_UNKNOWN;
		}
	}

	bool ATexture::LoadResource(std::string path)
	{
		Logger::Log(Logger::INFO, "Loading texture [%s]", path.c_str());
		SDL_Surface* surface = nullptr;
		bool bTextureLoadOK = false;

		std::filesystem::path fsPath = std::filesystem::path(path);

		if (!std::filesystem::exists(fsPath))
		{
			Logger::Log(Logger::WARN, "Unable to find texture at path [%s]", path);
			return false;
		}

		try {
			surface = IMG_Load(fsPath.generic_string().c_str());
			bTextureLoadOK = true;
		}
		catch (std::exception e)
		{
			Logger::Log(Logger::ERROR, "Exception was thrown whilst trying to load texture [%s].", fsPath.generic_string().c_str());
			bTextureLoadOK = false;
		}

		if (surface == nullptr)
		{
			bTextureLoadOK = false;
			Logger::Log(Logger::ERROR, "Error whilst loading texture [%s].", SDL_GetError());
			// Use default texture, return 
			// reminder that the texture needs assigning in platform native code
			//m_textureID = CreatePinkBlackTexture();
			LoadPinkBlackTexture();
			return false;
		}
		else
		{
			Logger::Log(Logger::VERBOSE, "Texture loaded successfully: [%s]", fsPath.generic_string().c_str());
		}

		// check format supported
		m_textureFormat = GetTextureFormat(surface);

		// Not supported, log and convert to RGBA
		if (TextureData::GetNativeTextureFormat(m_textureFormat) == -1)
		{
			Logger::Log(Logger::WARN, "Unsupported texture format, format being converted to RGBA");
			SDL_Surface* original = surface;

			SDL_ConvertSurface(surface, SDL_PixelFormat::SDL_PIXELFORMAT_RGBA8888);
			SDL_DestroySurface(original);
			m_textureFormat = TextureFormat::LEMONADE_RGBA8888;
		}

		LoadNativeTextureFromSurface(surface);

		if (m_bStorePixelData)
		{
			if (m_textureFormat == TextureFormat::LEMONADE_RGB888 || m_textureFormat == TextureFormat::LEMONADE_RGBA8888)
			{
				m_pixelData.clear();

				int increment = (m_textureFormat == TextureFormat::LEMONADE_RGB888) ? 3 : 4;
				unsigned char* pixels = static_cast<unsigned char*>(surface->pixels);

				for (int i = 0; i < surface->w * surface->h * increment; i += increment)
				{
					float r = pixels[i + 0];
					float g = pixels[i + 1];
					float b = pixels[i + 2];
					float a = 1;
					if (m_textureFormat == TextureFormat::LEMONADE_RGBA8888)
					{
						float a = ((unsigned char*)surface->pixels)[i + 3];
					}

					m_pixelData.push_back(Colour(r, g, b, a));
				}
			}
			else
			{
				Logger::Log(Logger::WARN, "Unsupported texture format.");
			}
		}

		m_width = surface->w;
		m_height = surface->h;

		SDL_DestroySurface(surface);
		// TODO - graphics error logging.
		//LogGraphicsErrors();
		return true;
	}

	void ATexture::LoadPinkBlackTexture()
	{
		if (m_bPinkBlackTextureLoaded)
		{
			return;
		}

		Logger::Log(Logger::VERBOSE, "Creating pink black debug texture.");

		// Write pink/ black missing texture 64X64
		for (int i = 0; i < 16; ++i)
		{
			for (int p = 0; p < 16; ++p)
			{
				if ((p % 2 == 0))
				{
					m_pixelData.push_back((i % 2 == 0) ? Colour(0, 0, 0, 255) : Colour(255, 0, 255, 255));
				}
				else
				{
					m_pixelData.push_back((i % 2 != 0) ? Colour(0, 0, 0, 255) : Colour(255, 0, 255, 255));
				}
			}
		}

		m_textureFilter = TextureFilter::NearestNeighbour;
		LoadNativeTextureFromPixels(m_pixelData);
		m_bPinkBlackTextureLoaded = true;
	}
}