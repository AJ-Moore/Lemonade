#pragma once

#include <LCommon.h>
#include <Core/Services/LService.h>
#include <Core/WindowManager/AWindow.h>

#ifdef RENDERER_OPENGL
#include <OGL/Renderer/Core/LGraphicsContext.h>
#elif RENDERER_VULKAN 
#include <Vulkan/Renderer/Core/LGraphicsContext.h>
#endif

namespace Lemonade
{
	class LEMONADE_API LWindowManager : public LService {
	public:
		LWindowManager(); 
		virtual ~LWindowManager(); 

		void AddWindow(std::shared_ptr<LWindow> window);
		std::shared_ptr<LWindow> AddWindow(CitrusCore::Rect<uint32> WindowRect, std::string windowCaption);

		/// Returns a reference to the default persistant window.
		std::shared_ptr<LWindow> GetMainWindow() { return m_defaultWindow; }

	protected:
		virtual bool Init() = 0;
		virtual void Unload();
		virtual void Update();
		virtual void Render();

	private:
		LGraphicsContext m_graphicsContext;
		std::unordered_map<uint32, std::shared_ptr<LWindow>> m_windows;
		std::shared_ptr<LWindow> m_defaultWindow;
	};
}
