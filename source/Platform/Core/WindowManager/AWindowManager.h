#pragma once

#include <Core/Services/LService.h>
#include <Core/WindowManager/LWindow.h>

namespace Lemonade
{
	class LEMONADE_API AWindowManager : public LService {
	public:
		AWindowManager(); 
		virtual ~AWindowManager(); 

		void AddWindow(std::shared_ptr<LWindow> window);

		/// Returns a reference to the default persistant window.
		std::shared_ptr<LWindow> GetMainWindow() { return m_window; }

	protected:
		virtual bool Init() = 0;
		virtual void Unload();
		virtual void Update();
		virtual void Render();

	private:

#ifdef RENDERER_OPENGL
		const SDL_GLContext& getContext() const { return glContext; }

		/// The GLContext used by all windows handeled by this window manager.
		SDL_GLContext glContext;
#endif

		std::unordered_map<uint32, std::shared_ptr<LWindow>> m_windows;
	};
}
