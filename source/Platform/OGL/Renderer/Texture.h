#pragma once

#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/ATexture.h>

#ifdef RENDERER_OPENGL

namespace Lemonade
{
	class LEMONADE_API Texture : ATexture
	{
	public:
		virtual void Bind();
		virtual void Bind(uint textureUnit);
		virtual void LoadNativeTextureFromSurface(SDL_Surface* surface);
		virtual void LoadNativeTextureFromPixels(const std::vector<Colour>& data);
	private:
		GLuint m_textureId;
	};
}

#endif