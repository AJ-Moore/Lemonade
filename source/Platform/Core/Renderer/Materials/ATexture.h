#pragma once

#include <LCommon.h>
#include <Resources/AResource.h>
#include <Platform/Core/Renderer/Materials/TextureUtils.h>
#include <cstdint>

namespace Lemonade
{
	class LEMONADE_API ATexture : public CitrusCore::AResource<ATexture>
	{
		friend class ResourceManager;
	public:
		virtual void Bind() = 0;
		virtual void Bind(uint textureUnit) = 0;

		const TextureFormat& GetTextureFormat() const { return m_textureFormat; }
		bool LoadedOK() const noexcept { return !m_bPinkBlackTextureLoaded; }
	protected:
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override{}
		TextureFormat GetTextureFormat(SDL_Surface* surface) const;
		virtual void LoadNativeTextureFromSurface(SDL_Surface* surface) = 0;
		virtual void LoadNativeTextureFromPixels(const Colour* data, uint32_t width, uint32_t height, TextureFormat textureformat = TextureFormat::LEMONADE_RGBA8888) = 0;

		uint m_width = 0; 
		uint m_height = 0;
	private:
		void LoadPinkBlackTexture();
		std::vector<Colour> m_pixelData;
		TextureFilter m_textureFilter = TextureFilter::Linear;
		TextureClamp m_textureClamp = TextureClamp::Repeat;
		bool m_bStorePixelData = false;
		TextureFormat m_textureFormat = TextureFormat::LEMONADE_UNKNOWN;
		bool m_bPinkBlackTextureLoaded = false;
	};
}