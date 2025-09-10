#include "ATexture.h"
#include "Platform/Core/Renderer/Materials/TextureUtils.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
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
			return TextureFormat::LEMONADE_FORMAT_UNKNOWN;
		}

		switch (surface->format) {
		case SDL_PIXELFORMAT_RGB24:
			return TextureFormat::LEMONADE_FORMAT_RGB888;
		case SDL_PIXELFORMAT_RGBA8888:
			return TextureFormat::LEMONADE_FORMAT_RGBA8888;
		case SDL_PIXELFORMAT_ABGR8888:
			return TextureFormat::LEMONADE_FORMAT_ABGR8888;
		case SDL_PIXELFORMAT_BGRA8888:
			return TextureFormat::LEMONADE_FORMAT_BGRA8888;
		case SDL_PIXELFORMAT_BGR24:
			return TextureFormat::LEMONADE_FORMAT_BGR888;
		case SDL_PIXELFORMAT_INDEX8:
			return TextureFormat::LEMONADE_FORMAT_R8;
		default:
			Logger::Log(Logger::ERROR, "Texture unsupported format: %s", SDL_GetPixelFormatName(surface->format));
			return TextureFormat::LEMONADE_FORMAT_UNKNOWN;
		}
	}

	TextureFormat ATexture::GetTextureFormatForType(TextureType texture)
	{
		switch (texture)
		{
			case(TextureType::Diffuse):
			case(TextureType::BaseColour):
				return TextureFormat::LEMONADE_FORMAT_RGBA8888;
			case(TextureType::Specular):
			case(TextureType::Normal):
			case(TextureType::NormalCamera):
				return TextureFormat::LEMONADE_FORMAT_RGB888;
			case(TextureType::Ambient):
			case(TextureType::Shininess):
			case(TextureType::Opacity):
			case(TextureType::Lightmap):
			case(TextureType::EmissionColour):
			case(TextureType::Emissive):
			case(TextureType::Metalness):
			case(TextureType::Roughness):
			case(TextureType::AmbientOcclusion):
			case(TextureType::Reflection):
				return TextureFormat::LEMONADE_FORMAT_R8;
			case(TextureType::Height):
			case(TextureType::Displacement):
				return TextureFormat::LEMONADE_FORMAT_R16;
			default:
				break;		
		}

		return TextureFormat::LEMONADE_FORMAT_RGBA8888;	
	}

	bool ATexture::LoadResource(std::string path)
	{
		Logger::Log(Logger::INFO, "Loading texture [%s]", path.c_str());
		SDL_Surface* surface = nullptr;
		bool bTextureLoadOK = false;

		std::filesystem::path fsPath = std::filesystem::path(path);

		if (!std::filesystem::exists(fsPath))
		{
			Logger::Log(Logger::WARN, "Unable to find texture at path [%s]", path.c_str());
			//dont't return here;
		}
		else {
			try {
				surface = IMG_Load(fsPath.generic_string().c_str());
				bTextureLoadOK = true;
			}
			catch (std::exception e)
			{
				Logger::Log(Logger::ERROR, "Exception was thrown whilst trying to load texture [%s].", fsPath.generic_string().c_str());
				bTextureLoadOK = false;
			}
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

		m_textureFormat = GetTextureFormat(surface);

		//Logger::Log(Logger::WARN, "Unsupported texture format, format being converted to RGBA");
		if (m_textureFormat == TextureFormat::LEMONADE_FORMAT_RGB888 || m_textureFormat == TextureFormat::LEMONADE_FORMAT_BGR888)
		{	
			SDL_Surface* rgbaSurface = SDL_CreateSurface(
				surface->w, surface->h, SDL_PIXELFORMAT_RGBA8888		
			);

			Uint8* src = (Uint8*)surface->pixels;
			Uint8* dst = (Uint8*)rgbaSurface->pixels;

			for (int y = 0; y < surface->h; y++) {
				for (int x = 0; x < surface->w; x++) {
					Uint8 r = src[y * surface->pitch + x * 3 + 0];
					Uint8 g = src[y * surface->pitch + x * 3 + 1];
					Uint8 b = src[y * surface->pitch + x * 3 + 2];
			
					dst[y * rgbaSurface->pitch + x * 4 + 0] = r;
					dst[y * rgbaSurface->pitch + x * 4 + 1] = g;
					dst[y * rgbaSurface->pitch + x * 4 + 2] = b;
					dst[y * rgbaSurface->pitch + x * 4 + 3] = 255;
				}
			}

			surface = rgbaSurface;
				
		}
		else if (m_textureFormat != TextureFormat::LEMONADE_FORMAT_ABGR8888)
		{
			surface = SDL_ConvertSurfaceAndColorspace(
				surface,
				SDL_PIXELFORMAT_RGBA8888,
				NULL,
				SDL_COLORSPACE_SRGB_LINEAR,
				0
			);
		}
		m_textureFormat = TextureFormat::LEMONADE_FORMAT_RGBA8888;

		LoadNativeTextureFromSurface(surface);

		if (m_bStorePixelData)
		{
			if (m_textureFormat == TextureFormat::LEMONADE_FORMAT_RGB888 || m_textureFormat == TextureFormat::LEMONADE_FORMAT_RGBA8888)
			{
				m_pixelData.clear();

				int increment = (m_textureFormat == TextureFormat::LEMONADE_FORMAT_RGB888) ? 3 : 4;
				unsigned char* pixels = static_cast<unsigned char*>(surface->pixels);

				for (int i = 0; i < surface->w * surface->h * increment; i += increment)
				{
					float r = pixels[i + 0];
					float g = pixels[i + 1];
					float b = pixels[i + 2];
					float a = 1;
					if (m_textureFormat == TextureFormat::LEMONADE_FORMAT_RGBA8888)
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
		for (int i = 0; i < 256; ++i)
		{
			for (int p = 0; p < 256; ++p)
			{
				if ((p % 16 < 8))
				{
					m_pixelData.push_back((i % 16 < 8) ? Colour(0, 0, 0, 255) : Colour(255, 0, 255, 255));
				}
				else
				{
					m_pixelData.push_back((i % 16 >= 8) ? Colour(0, 0, 0, 255) : Colour(255, 0, 255, 255));
				}
			}
		}

		m_textureFilter = TextureFilter::NearestNeighbour;
		m_textureFormat = TextureFormat::LEMONADE_FORMAT_RGBA8888;
		LoadNativeTextureFromPixels(m_pixelData.data(), 256, 256, m_textureFormat);
		m_bPinkBlackTextureLoaded = true;
	}
}