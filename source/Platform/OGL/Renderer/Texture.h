#pragma once

#include <LCommon.h>
#include <platform/core/Renderer/Materials/ATexture.h>

#ifdef RENDERER_OPENGL

namespace Lemonade
{
	class LEMONADE_API Texture : ATexture
	{
	public:
		virtual void Bind();
		virtual void Bind(uint textureUnit);
		virtual void LoadNativeTextureFromSurface(SDL_Surface* surface);
	private:
		GLint m_textureId;
	};
}

#endif