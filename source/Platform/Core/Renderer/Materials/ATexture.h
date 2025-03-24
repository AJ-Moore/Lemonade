#pragma once

#include <LCommon.h>
#include <Resources/AResource.h>
#include <Core/Renderer/Materials/TextureUtils.h>

namespace Lemonade
{
	class LEMONADE_API ATexture : public CitrusCore::AResource<ATexture>
	{
		friend class ResourceManager;
	public:
		virtual void Bind() = 0;
		virtual void Bind(uint textureUnit) = 0;
	protected:
		virtual bool LoadResource(std::string path);
		TextureFormat GetTextureFormat(SDL_Surface* surface) const;
		virtual int GetNativeTextureFormat(TextureFormat format) const = 0;
		virtual void LoadNativeTextureFromSurface(SDL_Surface* surface) = 0;
	private:
		void LoadPinkBlackTexture();
		std::vector<Colour> m_pixelData;
		uint m_width = 0; 
		uint m_height = 0;
		TextureFilter m_textureFilter = TextureFilter::Linear;
		TextureClamp m_textureClamp = TextureClamp::Repeat;
		bool m_bStorePixelData = false;
		TextureFormat m_textureFormat = TextureFormat::LEMONADE_UNKNOWN;
		bool m_bPinkBlackTextureLoaded = false;
	};
}