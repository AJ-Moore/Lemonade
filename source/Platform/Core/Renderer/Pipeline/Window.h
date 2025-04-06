#pragma once

#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/AViewport.h>
#include <unordered_set>
#include <Geometry/Rect.h>
#include <Platform/Core/Services/LService.h>

namespace Lemonade
{
	class Window : public LService
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
		std::unordered_set<std::shared_ptr<AViewport>> m_viewports;
		Recti m_windowRect = Recti(0,0,1920,1080);
		std::string m_windowCaption = "CraftyBlocks";
	};
}