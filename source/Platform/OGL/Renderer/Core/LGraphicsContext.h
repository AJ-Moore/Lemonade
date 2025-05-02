#pragma once
#include <LCommon.h>

#ifdef RENDERER_OPENGL
#include <Platform/Core/Renderer/Core/AGraphicsContext.h>

namespace Lemonade
{
	class LEMONADE_API LGraphicsContext : public AGraphicsContext
	{
	public:
		const SDL_GLContext& GetContext() const { return m_glContext; }
	protected:
		virtual bool Init () override;
		virtual void Update() override; 
		virtual void Unload() override;
		virtual void Render() override;
	private:
		SDL_GLContext m_glContext;
	};
}
#endif