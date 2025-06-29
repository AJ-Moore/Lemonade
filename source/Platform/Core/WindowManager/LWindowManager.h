#pragma once

#include <Geometry/Rect.h>
#include <LCommon.h>
#include <Platform/Core/Services/LService.h>
#include <Platform/Core/WindowManager/AWindow.h>
#include <memory>

#ifdef RENDERER_OPENGL
#include <Platform/OGL/Renderer/Core/LGraphicsContext.h>
#include <Platform/OGL/WindowManager/LWindow.h>
#elif defined(RENDERER_VULKAN)
#include <Platform/Vulkan/Renderer/Core/LGraphicsContext.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#endif

namespace Lemonade
{
	using CitrusCore::Rect;

	class LEMONADE_API LWindowManager : public LService {
	public:
		LWindowManager(); 
		virtual ~LWindowManager(); 

		void AddWindow(std::shared_ptr<LWindow> window);
		std::shared_ptr<LWindow> AddWindow(Rect<uint32> WindowRect, std::string windowCaption);

		/// Returns a reference to the default persistant window.
		std::shared_ptr<LWindow> GetMainWindow() { return m_defaultWindow; }
		LWindow* GetActiveWindow() { return m_activeWindow; }
		const std::vector<std::shared_ptr<LWindow>> GetWindows() { return m_windows; }

	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();
		bool Load();

	private:
		LGraphicsContext m_graphicsContext;
		std::vector<std::shared_ptr<LWindow>> m_windows;
		std::shared_ptr<LWindow> m_defaultWindow;
		LWindow* m_activeWindow;
	};
}
