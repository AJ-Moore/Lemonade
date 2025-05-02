#pragma once

#include <LCommon.h>
#ifdef RENDERER_VULKAN
#include <Platform/Core/Renderer/Materials/ATexture.h>


namespace Lemonade
{
	class LEMONADE_API Texture : ATexture
	{
	public:
		virtual void Bind(){}
		virtual void Bind(uint textureUnit){}
		virtual void LoadNativeTextureFromSurface(SDL_Surface* surface){}
		virtual void LoadNativeTextureFromPixels(const std::vector<Colour>& data){}
	};
}

#endif