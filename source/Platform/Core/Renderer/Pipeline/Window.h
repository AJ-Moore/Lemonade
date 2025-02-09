#pragma once

#include <Common.h>
#include <Renderer/Viewport.h>
#include <unordered_set>
#include <Util/Rect.h>
#include <Core/Services/Service.h>

namespace CraftyBlocks
{
	class Window : public Service
	{
	public:
		const SDL_GLContext& GetContext() const { return m_glContext; }
	protected:
		virtual bool Init() override;
		virtual void Render() override;
		virtual void Update() override; 
		virtual void Unload() override;
	private:

		SDL_Window* m_sdlWindow = nullptr;
		SDL_GLContext m_glContext;
		std::unordered_set<std::shared_ptr<Viewport>> m_viewports;
		uRect m_windowRect = uRect(0,0,1920,1080);
		std::string m_windowCaption = "CraftyBlocks";
	};
}